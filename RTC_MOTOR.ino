#include <Wire.h>       //I2C library
#include <RtcDS1307.h>  //RTC library


//============================================ Object ============================================================
//RtcDS3231 rtcObject;              //Uncomment for version 1.0.1 of the rtc library
RtcDS1307<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library

//============================================ Pins ==============================================================

int SWITCH_PIN = 2; // micro switch
int enA = 10; //motor
int in1 = 9;
int in2 = 8;

//============================================ Variable ==========================================================

String timeDayStart = ""; //For remember first time eggs incubator 
int timeHourStart = 99;
int timeMinuteStart = 99;
int timeSecondStart = 99;

int timeTurnH = 99; //time hour and minute for turn eggs
int timeTurnM = 99; 
int countDay = 0;

void setup() {
  Serial.begin(9600);

  //RTC
  rtcObject.Begin();   
  RtcDateTime currentTime = rtcObject.GetDateTime();
  timeDayStart = String(String(currentTime.Day())+"/"+String(currentTime.Month())+"/"+String(currentTime.Year()));
  timeHourStart = currentTime.Hour();
  timeMinuteStart = currentTime.Minute();
  timeSecondStart = currentTime.Second();
  
  timeTurnH = currentTime.Hour() + 1;
  timeTurnM = currentTime.Minute();

  

  //Motor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  analogWrite(enA, 255);
  motorSet();

  //Micro Switch
  pinMode(SWITCH_PIN,INPUT);

}
//============================================== Fouctions ================================================
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

void loop() {
  
  
  //RTC
  RtcDateTime currentTime = rtcObject.GetDateTime();
  //Count of day
  if(currentTime.Hour() == timeHourStart && timeMinuteStart ==  currentTime.Minute() && timeSecondStart == currentTime.Second()){
    countDay = countDay + 1;
  }
  //Turn eggs
  if(currentTime.Hour() == timeTurnH && currentTime.Minute() == timeTurnM && countDay <= 18){
    Serial.println("Time Turn Motor ..."); //delete
    motorTurn();
    timeTurnH = timeTurnH + 1;
  }
  
  char str[20];
  sprintf(str, "%d/%d/%d %d:%d:%d",     //%d allows to print an integer to the string
          currentTime.Year(),   //get year method
          currentTime.Month(),  //get month method
          currentTime.Day(),    //get day method
          currentTime.Hour(),   //get hour method
          currentTime.Minute(), //get minute method
          currentTime.Second()  //get second method
         );
  
  Serial.print(str); //print the string to the serial port
  Serial.print(" ");
  Serial.print(timeTurnH);
  Serial.print(":");
  Serial.println(timeTurnM);
  delay(1000);
  
}
