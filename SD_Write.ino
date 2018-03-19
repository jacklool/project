#include <SPI.h>
#include <SD.h>

const int CS_PIN = D8; // CS pin of NodeMCU.

void setup() {
  Serial.begin(9600);
  
  //Setup for SD Card.
  Serial.print("Initializing Card");
  pinMode(CS_PIN, OUTPUT);
  while(!Serial){
    ;
  }
  Serial.println("Initializing SD card...");

  if(!SD.begin(CS_PIN)){
    Serial.println("Card Failure");
    return;
  }
  Serial.println("Card Ready");

}

void loop() {
  
  //Open file and write to it.
  File dataFile = SD.open("log.csv", FILE_WRITE);
  if(dataFile){
    dataFile.print("Date, Time, Value");
    dataFile.close();

    Serial.println("Date, Time, Value"); // Debug delete
  }
  else{
    Serial.println("Couldn't opend log file");
  }
  delay(5000);
}
