#include <Wire.h>       //I2C library
#include <RtcDS1307.h>  //RTC library
#include "DHT.h"

//====================================== Pins ====================================
#define DHTPIN 7   //TEMP& HUM  
#define DHTTYPE DHT22   

int HEATER_PIN = 6;//Heater

//====================================== Object ==================================
RtcDS1307<TwoWire> rtcObject(Wire); //RTC
DHT dht(DHTPIN, DHTTYPE); //DHT22

//====================================== Variable ================================
//RTC
String timeDayStart = ""; //For remember first time eggs incubator 
int timeHourStart = 99;
int timeMinuteStart = 99;
int timeSecondStart = 99;

int timeTurnH = 99; //time hour and minute for turn eggs
int timeTurnM = 99; 
int countDay = 15;

int timeHourStop = 99;

int timeCountM = 99; //Delete

//Temp & Hum
float tempMax = 99.01;
int timeSinceLastRead = 0;
float tempDTH22 = 86;


//======================================= Set up() =============================
void setup() {
  Serial.begin(9600);
  
  //RTC
  rtcObject.Begin();   
  RtcDateTime currentTime = rtcObject.GetDateTime();
  timeDayStart = String(String(currentTime.Day())+"/"+String(currentTime.Month())+"/"+String(currentTime.Year()));
  timeHourStart = currentTime.Hour();
  timeMinuteStart = currentTime.Minute();
  timeSecondStart = currentTime.Second();
  
  //when time to 00:00
  if(timeTurnH + 1 > 24 ){
    timeTurnH = 0;
  }else{
    timeTurnH = currentTime.Hour() + 1;
  }
  timeTurnM = currentTime.Minute();
  //want Countday to 18.5
  if((timeHourStart + 12) > 24){
    timeHourStop = (timeHourStart + 12) - 24;
  }else{
    timeHourStop = timeHourStart + 12;
  }

  timeCountM = currentTime.Minute() + 1; //Delete
  
  //DHT22
  Serial.setTimeout(2000); //Delete
  while(!Serial) { } //Delete

  //Heater
  pinMode (HEATER_PIN, OUTPUT);

}
//======================================= Fouctions ==============================
void TempHum(){
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);
  tempDTH22 = f;
    
  if (isnan(h) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    timeSinceLastRead = 0;
    return;
  }
  Serial.print("Hum: ");
  Serial.print(h);
  Serial.print(" ");
  Serial.print("Temp: ");
  Serial.print(f);
  Serial.print(" ");
  Serial.print("TEMPMAX: ");
  Serial.println(tempMax);
  timeSinceLastRead = 0;
}
void ReadTime(){
  char str[20];
  RtcDateTime currentTime = rtcObject.GetDateTime();
  sprintf(str, "%d/%d/%d %d:%d:%d",     //%d allows to print an integer to the string
      currentTime.Year(),   //get year method
      currentTime.Month(),  //get month method
      currentTime.Day(),    //get day method
      currentTime.Hour(),   //get hour method
      currentTime.Minute(), //get minute method
      currentTime.Second()  //get second method
    );
  Serial.print(str);
  Serial.print(" TimeCount:");   
  Serial.print(timeCountM);  
  Serial.print(" CountDay:");   
  Serial.println(countDay);  
}
void loop() {
   //RTC
  RtcDateTime currentTime = rtcObject.GetDateTime();
  //Count of day
  /*if(currentTime.Hour() == timeHourStart && timeMinuteStart ==  currentTime.Minute() && timeSecondStart == currentTime.Second()){
    countDay = countDay + 1;
    
    //Temp up of Day
    if((countDay < 3) || (countDay >= 7 && countDay < 10) || (countDay >= 14 && countDay < 17)){
      tempMax + = 0.1;
    }
    if((countDay >= 3 && countDay < 7) || (countDay >= 10 && countDay < 14)){
      tempMax = = 0.075;
    }
  }*/
  
  if(timeCountM == currentTime.Minute()){
    countDay = countDay + 1;
    timeCountM += 1;

        //Temp up of Day
    if((countDay < 3) || (countDay >= 7 && countDay < 10) || (countDay >= 14 && countDay < 17)){
      tempMax -= 0.1;
    }
    if((countDay >= 3 && countDay < 7) || (countDay >= 10 && countDay < 14)){
      tempMax -= 0.075;
    }  
    if(countDay >= 17 && countDay < 19){
      tempMax -= 0.12;
    }
    if(countDay == 19){
      tempMax = 98.5;
    }
    if(countDay >= 20 && countDay < 21){
      tempMax -= 1;
    }  
  }

  if((countDay == 18 && timeHourStop == currentTime.Hour() && timeMinuteStart == currentTime.Minute() && timeSecondStart = currentTime.Second())){ //Have status motor;
    Serial.print("Motor turn and stop");
  }
  
  if(timeSinceLastRead > 5000){
    ReadTime();
  }
  if(timeSinceLastRead > 5000) {
    TempHum();
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
