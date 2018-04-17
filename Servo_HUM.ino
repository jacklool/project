#include <Servo.h>

const int SERVO = 11;
int posServo = 15;

int Hum = 55 ;

Servo myServo;
void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO);
  //myServo.write(55);

}
void servoMotor(int n1, int n2){
  int pos2 = 0;
  if(n2 < 0){
    if(n1 - 10 >= 15){
      pos2 = n1 - 10;
      Serial.println(pos2);
      for(int i = n1; i >= pos2; i--){
        myServo.write(i);
      }
      posServo -= 10;
    }else{
      Serial.println("->15");//delete
      myServo.write(15);
    }
  }else{
    if(n1 + 10 <= 45){
      pos2 = n1 + 10;
      Serial.println(pos2);
      for(int i = n1; i <= pos2; i++){
        myServo.write(i);
      }
      posServo += 10;      
    }else{
      Serial.println("->45");//delete
      myServo.write(45);
    }
  }
  
}
void loop() {
  if(Hum > 70){
    servoMotor(posServo, -1);
    Hum -= 1;
  }
  if(Hum < 60){
    servoMotor(posServo, 1);
    Hum += 1;
  }
  Serial.print("HUM:");
  Serial.println(Hum);
  delay(1000);

}
