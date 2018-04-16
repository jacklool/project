#include "DHT.h"

#define DHTPIN 7     
#define DHTTYPE DHT22   


int HEATER_PIN = 6;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); //Delete
  Serial.setTimeout(2000); //Delete
  while(!Serial) { } //Delete

  pinMode (HEATER_PIN, OUTPUT);
  
}

int timeSinceLastRead = 0;
float tempMax = 37.5;
float tempDTH22 = 30.5;

void loop() {
  if(timeSinceLastRead > 5000) {
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();  
    tempDTH22 = t;
    
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    Serial.print("Hum: ");
    Serial.print(h);
    Serial.print(" ");
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" ");
    Serial.print("TEMPMAX: ");
    Serial.println(tempMax);
    timeSinceLastRead = 0;
  }
  
  //Heater
  if(tempDTH22 >= tempMax){
    digitalWrite (HEATER_PIN, LOW);
  }else if(tempDTH22 < tempMax){
    digitalWrite (HEATER_PIN, HIGH);
  }
  
  delay(100);
  
  timeSinceLastRead += 100;


}
