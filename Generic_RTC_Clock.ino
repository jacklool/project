#include <SPI.h> //Library for Adafruit communication to OLED display
#include <Wire.h> //I2C communication library
#include "ds3231.h" //Real Time Clock library
#include <Adafruit_GFX.h> //Graphics library
#include <Adafruit_SSD1306.h> //OLED display library
#include <EEPROM.h> //This library allows reading and writing to the EEPROM


#define beeper //Uncomment if 5 volt continuous tone beeper or buzzer is connected to pin 10 (5 Hz output)

int framecount2 = 0; //Counter for number of display update periods
uint8_t secset = 0; //Index for second RTC setting
uint8_t minset = 1; //Index for minute RTC setting
uint8_t hourset = 2; //Index for hour RTC setting
uint8_t wdayset = 3; //Index for weekday RTC setting
uint8_t mdayset = 4; //Index for date RTC setting
uint8_t monset = 5; //Index for month RTC setting
uint8_t yearset = 6; //Index for year RTC setting

//Alarm time variables
uint8_t wake_HOUR = 0;
uint8_t wake_MINUTE = 0;
uint8_t wake_SECOND = 0;
uint8_t wake_SET = 1; //Default alarm to ON in case of power failure or reset

#define OLED_RESET 4 //Define reset for OLED display
Adafruit_SSD1306 display(OLED_RESET); //Reset OLED display
#define DS3231_I2C_ADDRESS 0x68
//Check for proper display size - required
//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

unsigned long prev, interval = 100; //Variables for display/clock update rate
byte flash = 0; //Flag for display flashing - toggle once per update interval
byte mode = 0; //Mode for time and date setting
int tempset; //Temporary variable for setting time/date
int beepcount = 0; //Variable for number of 100ms intervals since alarm started sounding
const int alarmEE = 0; //EEPROM alarm status storage location

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void setup()
{
Serial.begin(9600); //Initialize serial port, if needed (not used)
Wire.begin(); //Initialize I2C communication library
DS3231_init(0x00); //Initialize Real Time Clock for 1Hz square wave output (no RTC alarms on output pin)

pinMode(9, INPUT); //Set pin for time/date mode button to input
digitalWrite(9, HIGH); //Turn on pullup resistors

pinMode(10, INPUT); //Set pin for time/date set button to input
digitalWrite(10, HIGH); //Turn on pullup resistors

pinMode(11, OUTPUT); //Set pin for external alarm indicator output
digitalWrite(11, LOW); //Initialize external alarm to off state

//Read stored alarm set status and initialize to on at first run or on error  
wake_SET = EEPROM.read(alarmEE);
if (wake_SET != 0 && wake_SET != 1){
  wake_SET = 1;
  EEPROM.write(alarmEE, wake_SET);}
    
// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64 OLED display)
display.setTextSize(1); //Set default font size to the smalles
display.setTextColor(WHITE); //Set font to display color on black background
display.dim(0); //Set display to full brightness
display.invertDisplay(0); //Set display to normal video
// init done
}

void loop()
{
  
char tempF[6]; //Local variable to store converted temperature reading from Real Time Clock module
float temperature; //Intermediate temperature variable to convert Celsius to Farenheit
unsigned long now = millis(); //Local variable set to current value of Arduino internal millisecond run-time timer
struct ts t; //Structure for retrieving and storing time and date data from real time clock

//Draw and update display every refresh period (100ms)
if ((now - prev > interval)) { //Determine whether to start a time and screen update
  framecount2 = framecount2 + 1; //Update counter of refresh periods
  if(framecount2 > 300){
    framecount2 = 0; //Wrap the refresh period counter to 0 after 300 updates, 
    mode = 0; //Reset mode to normal every cycle unless setting buttons pressed to reset cycle counter
    }
  if(flash == 0){flash = 1;}else{flash = 0;} //Toggle flash flag for cursor blinking later
  DS3231_get(&t); //Get time and date and save in t structure
  get_alarm(); //Retrieve current alarm setting
  
  #if defined(dimming)
  if(t.hour >= 22 || t.hour < 5){display.dim(1);} //Dim the display between 10 PM and 5 AM
    else {display.dim(0);} //Otherwise set display to full brightness
  #endif

  #if defined(beeper)
  digitalWrite(11, LOW); //Turn off external alarm every cycle - no effect if alarm not on
  #endif
  
  #if defined(speaker)
  note(0,0); //Turn off external alarm beep every cycle - no effect if alarm not on
  #endif

  #if defined(voltage)
  if(!DS3231_triggered_a1()){digitalWrite(11, LOW);} //Turn off external alarm for flashing only if RTC alarm flag clear
  #endif

  display.invertDisplay(0); //Return display to normal after alarm stops - no effect if alarm not on
  
  //Force a temperature conversion if one is not in progress for rapid update and better clock accuracy
  //Maintain 1Hz square wave output
  if((DS3231_get_addr(0x0E) & 0x20) == 0){DS3231_init(0x20);} //Check for CONV flag to see if conversion is in progress first, else start conversion
  temperature = DS3231_get_treg(); //Get temperature from real time clock
  temperature = (temperature * 1.8) + 32.0; // Convert Celsius to Fahrenheit
  dtostrf(temperature, 5, 1, tempF); //Convert temperature to string for display

  display.clearDisplay(); //Clear display buffer from last refresh
  
//NOTE: Alarm indicators are overwritten in display buffer if full-screen animation is displayed, so no check for that  
  
  
  if(wake_SET && DS3231_triggered_a1()){ //Display/sound alarm if enabled and triggered
      beepcount = beepcount + 1;
      if(beepcount <= 600){ //Sound alarm for 60 seconds
        if(!flash){ //Flash display and sound interrupted beeper
        display.invertDisplay(1); //Flash inverse video display at 5 Hz
        
        #if defined(speaker)
        note(6, 6); //Sound external alarm if alarm triggered, regardless of mode (1480 Hz)on pin 10
        #endif

        #if defined(beeper)
        digitalWrite(11, HIGH); //Flash external alarm if alarm triggered, regardless of mode
        #endif

        #if defined(voltage)
        digitalWrite(11, HIGH); //Turn on external alarm if alarm triggered, regardless of mode
        #endif
        
        }
      }
      else{beepcount = 0; DS3231_clear_a1f();} //If alarm has sounded for 1 minute, reset alarm timer counter and alarm flag
  }

  if (mode <=7){ 
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
 
  display.setCursor(32,0);
  display.print(dayOfMonth, DEC);
  display.setCursor(75,0);
  display.print("20");
  display.print(year, DEC);

display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30,10);
  if(hour == 0){display.print("12");} //Convert zero hour for 12-hour display
    else if(hour < 13 && hour >= 10){display.print(hour);} //Just display hour if double digit hour
        else if(hour < 10){display.print(" "); display.print(hour);} //If single digit hour, add leading space
        else if(hour >= 13 && hour >= 22){display.print(hour-12);} //If double digit and PM, convert 24 to 12 hour
        else{display.print(" "); display.print(hour-12);} //If single digit and PM, convert to 12 hour and add leading space
        
        display.print(":"); //Display hour-minute separator
        if(minute<10){display.print("0");} //Add leading zero if single-digit minute
        display.print(minute); //Display retrieved minutes
        
        display.print(":"); //Display minute-seconds separator
        if(second<10){display.print("0");} //Add leading zero for single-digit seconds
        display.print(second); //Display retrieved seconds
        
        if(hour < 12){display.print(" AM");} //Display AM indicator, as needed
        else{display.print(" PM");} //Display PM indicator, as needed
//////////////////////////////FOR MONTH/////////////////////  
   switch(month)
   {
    case 1:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Jan");
    
    break;
    
    case 2:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Feb");
    
    break;

    case 3:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Mar");
    
    break;

    case 4:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Apr");
    
    break;

    case 5:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("May");
    
    break;

    case 6:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Jun");
    
    break;

    case 7:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Jul");
    
    break;

    case 8:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Aug");
    
    break;

    case 9:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Sep");
    
    break;

    case 10:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Oct");
    
    break;

    case 11:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Nov");
    
    break;

    case 12:
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(52,0);
    display.print("Dec");
    
    break;
   }

  }
    
    if (mode > 1){ 
      display.clearDisplay();
      display.setCursor(0, 0); //Position text cursor
      display.print("Alarm Set: ");
      if(wake_SET){display.print("ON");}else{display.print("OFF");}
      display.setCursor(0, 8); //Position text cursor for time display
        
      //RTC is operated in 24-hour mode and conversion to 12-hour mode done here, in software
      if(wake_HOUR == 0){display.print("12");} //Convert zero hour for 12-hour display
        else if(wake_HOUR < 13 && wake_HOUR >= 10){display.print(wake_HOUR);} //Just display hour if double digit hour
            else if(wake_HOUR < 10){display.print(" "); display.print(wake_HOUR);} //If single digit hour, add leading space
            else if(wake_HOUR >= 13 && wake_HOUR >= 22){display.print(wake_HOUR-12);} //If double digit and PM, convert 24 to 12 hour
            else{display.print(" "); display.print(wake_HOUR-12);} //If single digit and PM, convert to 12 hour and add leading space
        
            display.print(":"); //Display hour-minute separator
            if(wake_MINUTE<10){display.print("0");} //Add leading zero if single-digit minute
            display.print(wake_MINUTE); //Display retrieved minutes
        
            display.print(":"); //Display minute-seconds separator
            if(wake_SECOND<10){display.print("0");} //Add leading zero for single-digit seconds
            display.print(wake_SECOND); //Display retrieved seconds
        
            if(wake_HOUR < 12){display.print(" AM");} //Display AM indicator, as needed
            else{display.print(" PM");} //Display PM indicator, as needed
            
    }
    

 
//Time/Date setting button processing and cursor flashing
//CURSOR COORDINATES ARE SET TO MATCH TIME/DATE FIELD - DO NOT CHANGE!!
//Digital and analog time/date display updates with new settings at 5Hz as settings are changed
  switch(mode)
  {

    case 2: //Alarm hour setting
    
      if(flash){display.drawRect(0,8,12,8,WHITE);} //Display rectangle cursor every other display update (5Hz blink)
      if(!digitalRead(10) && (!flash)){ //Update setting at 5Hz rate if button held down
        tempset = wake_HOUR; //Get the current hour and save in temporary variable
        tempset = tempset + 1; //Increment the hour at 5Hz rate
        if(tempset > 23){tempset = 0;} //Roll over hour after 23rd hour (setting done in 24-hour mode)
        wake_HOUR = tempset; //After each update, write the hour back to the alarm variable
        set_alarm(); //Write the alarm setting back to the RTC after each update
      }
    break;
    
    
    case 3: //Alarm minute setting
      if(flash){display.drawRect(18,8,12,8,WHITE);} //Display rectangle cursor every other display update (5Hz blink)
      if(!digitalRead(10) && (!flash)){ //Update setting at 5Hz rate if button held down
        tempset = wake_MINUTE; //Get the current minute and save in temporary variable
        tempset = tempset + 1; //Increment the minute at 5Hz rate
        if(tempset > 59){tempset = 0;} //Roll over minute to zero after 59th minute
        wake_MINUTE = tempset; //After each update, write the minute back to the alarm variable
        set_alarm(); //Write the alarm setting back to the RTC after each update
      }
    break;
    
     case 4: //Alarm enable/disable
      if(flash){display.drawRect(66,0,18,8,WHITE);} //Display rectangle cursor every other display update (5Hz blink)
      if(!digitalRead(10) && (!flash)){ //Update setting at 5Hz rate if button held down
        if(wake_SET){wake_SET = 0;}else{wake_SET = 1;} //Toggle alarm on/of variable at 5 Hz
        EEPROM.write(alarmEE, wake_SET); //Save alarm enable setting to EEPROM
      }
    break;
  }
        
prev = now; //Reset variable for display and time update rate
display.display(); //Display the constructed frame buffer for this framecount
    }
    
//Clock setting mode set - outside time/display update processing for faster button response
if(!digitalRead(9)){ //Read setting mode button
  delay(25); //100ms debounce time
  if(!digitalRead(9)){ //Activate setting mode change after 100ms button press
    mode = mode + 1; //Increment the time setting mode on each button press
    framecount2 = 0;  //Reset cycle counter if button pressed to delay auto return to normal mode
    if(mode > 5){mode = 0;} //Roll the mode setting after 7th mode
    while(!digitalRead(9)){} //Wait for button release (freezes all display processing and time updates while button held, but RTC continues to keep time)
  }
 }
 
 if(!digitalRead(10)){ //Reset alarm flag if set button pressed
   delay(25); //25ms debounce time
   if(!digitalRead(10)){
    DS3231_clear_a1f(); //Reset cycle counter if button pressed to delay auto return to normal mode
    beepcount = 0; //Reset alarm timeout counter if alarm stopped by pushing button
    framecount2 = 0; 
    } 
 }
}



//Function to display month string from numerical month argument
void printMonth(int month)
{
  switch(month)
  {
    case 1: display.print("Jan ");break;
    case 2: display.print("Feb ");break;
    case 3: display.print("Mar ");break;
    case 4: display.print("Apr ");break;
    case 5: display.print("May ");break;
    case 6: display.print("Jun ");break;
    case 7: display.print("Jul ");break;
    case 8: display.print("Aug ");break;
    case 9: display.print("Sep ");break;
    case 10: display.print("Oct ");break;
    case 11: display.print("Nov ");break;
    case 12: display.print("Dec ");break;
    default: display.print("--- ");break; //Display dashes if error - avoids scrambling display
  } 
}


//Function to display day-of-week string from numerical day-of-week argument
void printDay(int day)
{
  switch(day)
  {
    case 1: display.print("Mon ");break;
    case 2: display.print("Tue ");break;
    case 3: display.print("Wed ");break;
    case 4: display.print("Thu ");break;
    case 5: display.print("Fri ");break;
    case 6: display.print("Sat ");break;
    case 7: display.print("Sun ");break;
    default: display.print("--- ");break; //Display dashes if error - avoids scrambling display
  } 
}

//Subroutine to adjust a single date/time field in the RTC
void set_rtc_field(struct ts t,  uint8_t index)
{
    uint8_t century;

    if (t.year > 2000) {
        century = 0x80;
        t.year_s = t.year - 2000;
    } else {
        century = 0;
        t.year_s = t.year - 1900;
    }

    uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };

    Wire.beginTransmission(DS3231_I2C_ADDR);
    Wire.write(index);
        TimeDate[index] = dectobcd(TimeDate[index]);
        if (index == 5){TimeDate[5] += century;}
        Wire.write(TimeDate[index]);
    Wire.endTransmission();
    
    //Adjust the month setting, per data sheet, if the year is changed
    if (index == 6){
      Wire.beginTransmission(DS3231_I2C_ADDR);
      Wire.write(5);
      TimeDate[5] = dectobcd(TimeDate[5]);
      TimeDate[5] += century;
      Wire.write(TimeDate[5]);
      Wire.endTransmission();
    } 
}


//Subroutine to set alarm 1
void set_alarm()
{

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm - see datasheet
    // A1M1 (seconds) (0 to enable, 1 to disable)
    // A1M2 (minutes) (0 to enable, 1 to disable)
    // A1M3 (hour)    (0 to enable, 1 to disable) 
    // A1M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    byte flags[5] = { 0, 0, 0, 1, 1 }; //Set alarm to trigger every 24 hours on time match

    // set Alarm1
    DS3231_set_a1(0, wake_MINUTE, wake_HOUR, 0, flags); //Set alarm 1 RTC registers

}

//Subroutine to get alarm 1
void get_alarm()
{
    uint8_t n[4];
    uint8_t t[4];               //second,minute,hour,day
    uint8_t f[5];               // flags
    uint8_t i;

    Wire.beginTransmission(DS3231_I2C_ADDR);
    Wire.write(DS3231_ALARM1_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_I2C_ADDR, 4);

    for (i = 0; i <= 3; i++) {
        n[i] = Wire.read();
        f[i] = (n[i] & 0x80) >> 7;
        t[i] = bcdtodec(n[i] & 0x7F);
    }

    f[4] = (n[3] & 0x40) >> 6;
    t[3] = bcdtodec(n[3] & 0x3F);
    
    wake_SECOND = t[0];
    wake_MINUTE = t[1];
    wake_HOUR = t[2];
}

#if defined(__AVR_ATmega32U4__) && defined(speaker)
//Subroutine for low overhead beep for alarm speaker on pin 10, ATmega32U4
const uint8_t scale[] PROGMEM = {239,225,213,201,190,179,169,159,150,142,134,127};

void note (int n, int octave) {
  DDRB = DDRB | 1<<DDB6;                     // PB6 (Arduino D10) as output
  TCCR4A = 0<<COM4A0 | 1<<COM4B0;            // Toggle OC4B on match
  int prescaler = 10 - (octave + n/12);
  if (prescaler<1 || prescaler>9) prescaler = 0;
  OCR4C = pgm_read_byte(&scale[n % 12]);
  TCCR4B = prescaler<<CS40;
}

#elif !defined(__AVR_ATmega32U4__) && defined(speaker)
//Subroutine for low overhead beep for alarm speaker on pin 10, ATmega328
const uint8_t scale[] PROGMEM = {239,225,213,201,190,179,169,159,150,142,134,127};

void note (int n, int octave) {
  DDRD = DDRD | 1<<DDD3;                     // PD3 (Arduino D3) as output
  TCCR2A = 0<<COM2A0 | 1<<COM2B0 | 2<<WGM20; // Toggle OC2B on match
  int prescaler = 9 - (octave + n/12);
  if (prescaler<3 || prescaler>6) prescaler = 0;
  OCR2A = pgm_read_byte(&scale[n % 12]);
  TCCR2B = 0<<WGM22 | prescaler<<CS20;
}
#endif

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
