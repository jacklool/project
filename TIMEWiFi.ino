#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "TP-LINK"; //ใส่ชื่อ SSID Wifi
const char* password = "0896088725001"; //ใส่รหัสผ่าน

int timezone = 7 * 3600; //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0; //กำหนดค่า Date Swing Time

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  WiFi.mode(WIFI_STA); //เชื่อมต่อ Wifi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("*");
    delay(1000);
  }
  
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov"); //ดึงเวลาจาก Server
  Serial.println("\nLoading time");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("");
}


void loop() {
  // put your main code here, to run repeatedly:
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.print(p_tm->tm_sec);
  Serial.println("");
  delay(1000);
  
}
