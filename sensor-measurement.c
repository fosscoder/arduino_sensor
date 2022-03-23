int sensorValue = analogRead(A1);

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module


double Voltagei = 0;
double VRMSi = 0;
double AmpsRMS = 0;



void setup(){ 
 Serial.begin(9600);
}

void loop(){
 
 
 Voltagei = getVPPi();
 VRMSi = (Voltagei/1.414); // convert peak value to rms value
 AmpsRMS = (VRMSi * 1000)/mVperAmp;
 Serial.print(AmpsRMS);
 delay(10000);  // delay of 10 seconds
}

float getVPPi()
{
  float resultc;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorValue);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   resultc = ((maxValue - minValue) * 5.0)/1024.0;
      
   return resultc;
 }
