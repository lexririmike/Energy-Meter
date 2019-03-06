
#include <EmonLib.h>   // Include Emon Library

//16bvg = 8192 pages;

EnergyMonitor ct1, ct2, ct3, ct4;              // Create an instance
 
 float power1    = 0; 
 float power2    = 0; 
 float power3    = 0; 
 float power4    = 0; 
 float powerFActor1    = 0; 
  float powerFActor2   = 0;
  float powerFActor3    = 0;
  float powerFActor4    = 0;
  float realPower1     = 0;  
  float realPower2      = 0;  
  float realPower3      = 0;  
  float realPower4      = 0;  
  float apparentPower1   = 0; 
  float apparentPower2   = 0; 
  float apparentPower3   = 0; 
  float apparentPower4   = 0; 
  float supplyVoltage   = 0; 
   float Irms1          = 0;  
   float Irms2           = 0;           
   float Irms3           = 0;  
   float Irms4           = 0;  

boolean CT1, CT2, CT3, CT4, ACAC;
  byte CT_count=0;
  const int no_of_half_wavelengths= 30;   
  unsigned long start=0;
  float Vcal = 281.99;
  float Ical = 119.16;
char server_header[] = "http://example.com/input/post?apikey=YourKey&node=mitalogicemontx&json={";//power1:100,power2:200,power3:300}


int gsm_reset_pin = 5;
void setup() {
  pinMode(gsm_reset_pin,OUTPUT);
  Serial.begin(9600);
 
 double vrms = calc_rms(4,1780) * (Vcal * (3.3/1024) );
  if (vrms>90) ACAC = 1; else ACAC=0;
  
 if (analogRead(1) > 0) {CT1 = 1; CT_count++;} else CT1=0;              // check to see if CT is connected to CT1 input, if so enable that channel
  if (analogRead(2) > 0) {CT2 = 1; CT_count++;} else CT2=0;              // check to see if CT is connected to CT2 input, if so enable that channel
  if (analogRead(3) > 0) {CT3 = 1; CT_count++;} else CT3=0;              // check to see if CT is connected to CT3 input, if so enable that channel
  if (analogRead(0) > 0) {CT4 = 1; CT_count++;} else CT4=0;              // check to see if CT is connected to CT4 input, if so enable that channel

  if ( CT_count == 0) CT1=1;                                             // If no CT's are connect ed CT1-4 then by default read from CT1

 if (ACAC)
  {
 //voltage pin
   ct1.voltage(4, Vcal, 1.7);   // Voltage1: input pin, calibration, phase_shift
   ct2.voltage(4, Vcal, 1.7);   // Voltage2: input pin, calibration, phase_shift
   ct3.voltage(4, Vcal, 1.7);   // Voltage3: input pin, calibration, phase_shift
   ct4.voltage(4, Vcal, 1.7);   // Voltage4: input pin, calibration, phase_shift
  }
   //current pin
   ct1.current(1, Ical);        // Current1: input pin, calibration((2000 turns / 22 Ohm burden) = 90.9).
  ct2.current(2, Ical);        // Current2: input pin, calibration((2000 turns / 22 Ohm burden) = 90.9).
  ct3.current(3, Ical);        // Current3: input pin, calibration((2000 turns / 22 Ohm burden) = 90.9).
  ct4.current(0, Ical);        // Current4: input pin, calibration(((2000 turns / 22 Ohm burden) = 90.9).


   ping_GSM();
  delay(100);
  initGPRS();
}

boolean state= false;
void loop() {

  if (ACAC) {
    delay(200);                         //if powering from AC-AC adapter allow time for power supply to settle
    supplyVoltage   = 0;                     //Set Vrms to zero, this will be overwirtten by CT 1-4
  }
  
  
  
    
      
  get_emondata();
  upload_data();
 
   //int pin=12;
  //pinMode(pin, OUTPUT); 
 
  
}
void read_serial() {
while (Serial.available() > 0)
{
Serial.read();
}
}
void ping_GSM()
{
   Serial.println("ATE0\r");
   delay(500);
  for(int k= 0;k<10;k++){
    Serial.println("AT\r");
    delay(200);
  }
}
void get_emondata()
{
    // Calculate all. 
 if (CT1){
  ct1.calcVI(no_of_half_wavelengths,2000);  
  ct1.serialprint();
   powerFActor1    = ct1.powerFactor; 
   
    apparentPower1   = ct1.apparentPower; 
      Irms1          = ct1.Irms;
      realPower1     = ct1.realPower; 
      power1= realPower1;
 }
 if (CT2){
   ct2.calcVI(no_of_half_wavelengths,2000);
   ct2.serialprint();
    powerFActor2    = ct2.powerFactor; 
       realPower2     = ct2.realPower;  
        apparentPower2   = ct2.apparentPower;
          Irms2          = ct2.Irms; 
          power2= 230*Irms2;
 }
 if (CT3){
    ct3.calcVI(no_of_half_wavelengths,2000);
    ct3.serialprint();
     powerFActor3   = ct3.powerFactor; 
        realPower3     = ct3.realPower;  
         apparentPower3   = ct3.apparentPower; 
           Irms3          = ct3.Irms;
           power3= 230*Irms3;
 }
 if (CT4){
     ct4.calcVI(no_of_half_wavelengths,2000);  
     ct4.serialprint();
      powerFActor4    = ct4.powerFactor; 
         realPower4     = ct4.realPower;  
          apparentPower4   = ct4.apparentPower; 
            Irms4          = ct4.Irms;
            power3= 230*Irms3;
            
 }
         
 supplyVoltage   = ct1.Vrms; 
delay(100);

}
 void initGPRS() {
  Serial.flush();
  Serial.println("AT+SAPBR=0,1\r");
  delay(2000);
  Serial.print("AT+HTTPTERM");
  delay(2000);
  Serial.print("\r\n");
  Serial.println("AT+SAPBR=3,1,\"APN\",\"iot-eu.aer.net\"\r");//set APN
 // delay(2000);
  Serial.println("AT+SAPBR=3,1,\"USER\",\"saf\"\r"); // set USer
  delay(1500);
 // read_serial();
 Serial.println("AT+SAPBR=3,1,\"PWD\",\"data\"\r"); // set password
  delay(1500);
//  read_serial();
  Serial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r");
  delay(2000);
  Serial.println("AT+SAPBR=1,1\r");
  delay(2000);
  Serial.print("AT+HTTPINIT");
  Serial.print("\r\n");
  delay(2000);
}
void upload_data()
  {
    Serial.flush();
    Serial.print("AT+HTTPINIT");
  Serial.print("\r\n");
  delay(150);
   Serial.println("AT+HTTPPARA=\"CID\",1\r");
  delay(150);
  Serial.flush();
  Serial.print("AT+HTTPPARA=\"URL\",\"");
  Serial.print(server_header);
  Serial.print("powerFActor1:");
  Serial.print(powerFActor1);
  Serial.print(",powerFActor2:");
  Serial.print(powerFActor2);
  Serial.print(",powerFActor3:");
  Serial.print(powerFActor3);
  Serial.print(",powerFActor4:");
  Serial.print(powerFActor4);
  Serial.print(",realPower1:");
  Serial.print(realPower1);
  Serial.print(",realPower2:");
  Serial.print(realPower2);
  Serial.print(",realPower3:");
  Serial.print(realPower3);
  Serial.print(",realPower4:");
  Serial.print(realPower4);
  Serial.print(",apparentPower1:");
  Serial.print(apparentPower1);
  Serial.print(",apparentPower2:");
  Serial.print(apparentPower2);
  Serial.print(",apparentPower3:");
  Serial.print(apparentPower3);
  Serial.print(",apparentPower4:");
  Serial.print(apparentPower4);
  Serial.print(",supplyVoltage:");
  Serial.print(supplyVoltage);
   Serial.print(",Irms1:");
  Serial.print(Irms1);
  Serial.print(",Irms2:");
  Serial.print(Irms2);
  Serial.print(",Irms3:");
  Serial.print(Irms3);
  Serial.print(",Irms4:");
  Serial.print(Irms4);
  Serial.print(",RawADC:");
  Serial.print(analogRead(4));
  Serial.print('}');
Serial.print("\"\r\n");
//totalMilliLitres
//sensorValue
delay(200);
Serial.println("AT+HTTPACTION=1\r");//post data to server
delay(300);
read_serial();
Serial.println("AT+HTTPREAD");//read data from server
delay(500);
read_serial();
}

double calc_rms(int pin, int samples)
{
  unsigned long sum = 0;
  for (int i=0; i<samples; i++) // 178 samples takes about 20ms
  {
    int raw = (analogRead(0)-512);
    sum += (unsigned long)raw * raw;
  }
  double rms = sqrt((double)sum / samples);
  return rms;
}


