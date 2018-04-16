
int SWITCH_PIN = 2;

int enA = 10;
int in1 = 9;
int in2 = 8;

//Variable
char statusMotor = 'F';


void setup() {
  Serial.begin(9600);
  
  //Pin of motor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  analogWrite(enA, 255);

  //Pin of swicth
  pinMode(SWITCH_PIN,INPUT);
  //motorSet();
  //motorTurn();
}
//============================================ Fouction ================================================
void motorSet(){
  Serial.println("Fouction motor set...");//delete
  while(1){
    Serial.println("InLoop");//delete
    if(digitalRead(SWITCH_PIN) == HIGH)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      Serial.println("Motor Stop");
      break;
    }
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
}

void motorTurn(){
  Serial.println("Fouction motor Turn...");//delete
  
  if(digitalRead(SWITCH_PIN) == HIGH){
    
    Serial.println("Swith == HIGH");//delete
    digitalWrite(in1,LOW);
    digitalWrite(in2, HIGH);
    delay(4500);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    Serial.println("STOP_HIGH");//delete
    return;
    
  }else if(digitalRead(SWITCH_PIN) == LOW){
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

void loop() {
  // put your main code here, to run repeatedly:
  //motorTurn();
}
