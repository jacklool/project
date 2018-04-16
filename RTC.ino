#include <Wire.h>       //I2C library
#include <RtcDS1307.h>  //RTC library
 
//RtcDS3231 rtcObject;              //Uncomment for version 1.0.1 of the rtc library
RtcDS1307<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library

//============================================ Variable ==========================================================


String timeDayStart = "";
int timeHourStart = 99;
int timeMinuteStart = 99;
int timeSecondStart = 99;

int timeTurnH = 99;
int timeTurnM = 99;
int countDay = 0;
 
void setup() {
 
  Serial.begin(9600);  //Starts serial connection
  rtcObject.Begin();     //Starts I2C
 
  RtcDateTime currentTime = RtcDateTime(18, 04, 12, 1, 48, 45); //define date and time object
  rtcObject.SetDateTime(currentTime); //configure the RTC with object

  //Time First
  //RtcDateTime currentTime = rtcObject.GetDateTime();
  timeDayStart = String(String(currentTime.Day())+"/"+String(currentTime.Month())+"/"+String(currentTime.Year()));
  timeHourStart = currentTime.Hour();
  timeMinuteStart = currentTime.Minute();
  timeSecondStart = currentTime.Second();
  
  timeTurnH = currentTime.Hour() + 1;
  timeTurnM = currentTime.Second();
  
}
 
void loop() {
 
  RtcDateTime currentTime = rtcObject.GetDateTime();    //get the time from the RTC
 
  char str[20];   //declare a string as an array of chars

  //Count day of eggs
  if(currentTime.Hour() == timeHourStart && timeMinuteStart ==  currentTime.Minute() && timeSecondStart == currentTime.Second()){
    countDay = countDay + 1;
  }

  //Time of Turn egg
  if(currentTime.Hour() == timeTurnH && currentTime.Minute() == timeTurnM){
    Serial.println("Time Turn Motor ...");
    timeTurnH = timeTurnH + 1;
  }



  
 
  sprintf(str, "%d/%d/%d %d:%d:%d",     //%d allows to print an integer to the string
          currentTime.Year(),   //get year method
          currentTime.Month(),  //get month method
          currentTime.Day(),    //get day method
          currentTime.Hour(),   //get hour method
          currentTime.Minute(), //get minute method
          currentTime.Second()  //get second method
         );
  
  Serial.println(str); //print the string to the serial port
  Serial.println(timeDayStart);
 
  delay(1000); //20 seconds delay
 
}




//Time 18
//Time Temp of day
//Time reset
