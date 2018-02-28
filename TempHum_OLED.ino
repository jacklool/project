#include <Wire.h>  //include of OLED
#include <ACROBOTIC_SSD1306.h>  
#include "DHT.h" //include of temperature and humidity
#define DHTPIN 0
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//Variable of temperature and humidity
int timeSinceLastRead = 0;

void setup() {
  //setip board
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
  
  //setup of OLED
  Wire.begin();
  oled.init();
  oled.clearDisplay();
  oled.setTextXY(0, 0);
  oled.putString("TEMP & HUMIDITY");
 
}

void loop() {
  
  //TEMPERATURE & HUMIDITY
  if(timeSinceLastRead > 2000){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }
    oled.setTextXY(2, 1);
    oled.putNumber(t);
    oled.setTextXY(2, 10);
    oled.putNumber(h);
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}
