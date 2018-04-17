const int sampleWindow = 50;                              
unsigned int sample;
void setup() {
   Serial.begin(9600);                           
}

void loop() {
   unsigned long startMillis= millis();           
   float peakToPeak = 0;                                  
   unsigned int signalMax = 0;                            
   unsigned int signalMin = 1024;                         
                                                          
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A0);                             
      if (sample < 1024)                                  
      {
         if (sample > signalMax)
         {
            signalMax = sample;                          
         }
         else if (sample < signalMin)
         {
            signalMin = sample;                           
         }
      }
   }
   peakToPeak = signalMax - signalMin;                    
   float db = map(peakToPeak,20,900,38.5,120);                          
   Serial.println(db);
}
