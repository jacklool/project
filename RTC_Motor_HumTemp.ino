#include <Wire.h>       //I2C library
#include <RtcDS1307.h>  //RTC library
#include "DHT.h" //DHT

//============================================ Pins ==============================================================

int SWITCH_PIN = 2; // micro switch
int enA = 10; //motor
int in1 = 9;
int in2 = 8;

#define DHTPIN 7   //TEMP& HUM  
#define DHTTYPE DHT22   

//============================================ Object ============================================================

RtcDS1307<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library
DHT dht(DHTPIN, DHTTYPE); //DHT22

//============================================ Variable ==========================================================
//RTC
String timeDateStart = ""; //For remember first time eggs incubator 
int timeDayStart = 99; 
int timeHourStart = 99;
int timeMinuteStart = 99;
int timeSecondStart = 99;
int timeTurnH = 99; //time hour and minute for turn eggs
int timeTurnM = 99; 
int countDay = 0;
int timeHourStop = 99;

//Temp & Hum
float tempMax = 99.01;
int timeSinceLastRead = 0;
float tempDTH22 = 86;

//============================================ Status ============================================================.
//RTC
String timeDay18 = "F"; 

//============================================ Set up() ==========================================================
void setup() {
  Serial.begin(9600); //delete

  //RTC
  rtcObject.Begin();   
  RtcDateTime currentTime = rtcObject.GetDateTime();
  timeDateStart = String(String(currentTime.Day())+"/"+String(currentTime.Month())+"/"+String(currentTime.Year()));
  timeDayStart = currentTime.Day();
  timeHourStart = currentTime.Hour();
  timeMinuteStart = currentTime.Minute();
  timeSecondStart = currentTime.Second();
  timeTurnM = currentTime.Minute();
  //when time to 00:00
  if(timeTurnH + 1 > 24 ){
    timeTurnH = 0;
  }else{
    timeTurnH = currentTime.Hour() + 1;
  }
  //want Countday to 18.5
  if((timeHourStart + 12) > 24){
    timeHourStop = (timeHourStart + 12) - 24;
  }else{
    timeHourStop = timeHourStart + 12;
  }
  //Motor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  analogWrite(enA, 255);
  motorSet();

  //DHT22
  Serial.setTimeout(2000); //Delete
  while(!Serial) { } //Delete
  
  //Micro Switch
  pinMode(SWITCH_PIN,INPUT);

}
//============================================== Fouctions ================================================
//Motor to Deferent
void motorSet(){
  Serial.println("Fouction motor set...");//delete
  while(1){
    Serial.println("InLoop");//delete
    if(digitalRead(SWITCH_PIN) == HIGH)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      Serial.println("Motor Stop");
      return;
    }
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
}
//Turn eggs to 45
void motorTurn(){
  Serial.println("Fouction motor Turn...");//delete
  while(1){
    if(digitalRead(SWITCH_PIN) == HIGH){
      Serial.println("Swith == HIGH");//delete
      digitalWrite(in1,LOW);
      digitalWrite(in2, HIGH);
      delay(3800);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      Serial.println("STOP_HIGH");//delete
      return;
    }
    if(digitalRead(SWITCH_PIN) == LOW){
      Serial.println("Swith == LOW");//delete
      while(1){
        if(digitalRead(SWITCH_PIN) == HIGH){
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          Serial.println("STOP_LOW");//delete
          return;
        }
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
      }  
    }
  }
}
//motor half
void motorHalf(){
   Serial.println("Fouction motor Half...");//delete
   motorSet();
   Serial.println("Half");//delete
   digitalWrite(in1,LOW);
   digitalWrite(in2, HIGH);
   delay(1700);
   digitalWrite(in1, LOW);
   digitalWrite(in2, LOW);
   Serial.println("STOP_Half");//delete
}
//Temp & Hum
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
//Read Time
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
  Serial.print(" CountDay:");   
  Serial.print(countDay);
  Serial.print(" timeHourStop:");   
  Serial.println(timeHourStop);   
}

void loop() {
  //RTC
  RtcDateTime currentTime = rtcObject.GetDateTime();

  //Count of day
  if((currentTime.Hour() == timeHourStart) && (timeMinuteStart ==  currentTime.Minute()) && (timeSecondStart == currentTime.Second()) && (timeDayStart != currentTime.Day()) ){
    countDay = countDay + 1;

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
  
  //Stop motor
  if((countDay == 18) && (timeHourStop == currentTime.Hour()) && (timeMinuteStart == currentTime.Minute()) && timeDay18 == "F"){ //Have status motor;
    timeDay18 = "T";
    motorHalf();
    Serial.println("Motor turn and stop"); //delete
  }  
  
  //Turn eggs
  if(currentTime.Hour() == timeTurnH && currentTime.Minute() == timeTurnM && countDay <= 18){
    Serial.println("Time Turn Motor ..."); //delete
    motorTurn();
    timeTurnH = timeTurnH + 1;
  }
  
  if(timeSinceLastRead > 5000){
    ReadTime();
  }
  if(timeSinceLastRead > 5000) {
    TempHum();
  }  
  
}
