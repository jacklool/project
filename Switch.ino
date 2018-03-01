//micro Swith 
#define LEVER_SWITCH_PIN 5
int pressSwitch = 0;
void setup()
{
Serial.begin(115200); //Rate
}
 
void loop()
{
  pinMode(LEVER_SWITCH_PIN,INPUT);
  pressSwitch = digitalRead(LEVER_SWITCH_PIN);
  if(pressSwitch == HIGH)
  {
    Serial.println("Pass Switch!");
    delay(1000);
  }
}
