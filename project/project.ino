#include <DHT.h>
#include <LiquidCrystal.h>
#define DHT22_PIN 3
#define DHTTYPE DHT22

////modül////
// pin definition
#define PWRKEY        4
#define EMERG         5
#define GSM_ON        6
//////////////
String GSM_string = "\r\n\n\nTest Protocol: GSM-Modul\r\n------------------------\r\n";

DHT dht22(DHT22_PIN, DHTTYPE);

const int buzzer = 13;
const int AOUTpin = A1;
const int DOUTpin = 2;
const int red = 3;
const int green = 4;
int temperature;
int limit;
int value;

int k = 0;
int m = 0;

String GAS_ALERT = "high gas level";
String TEMP_ALERT = "high temperature level";

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); 

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  pinMode(DOUTpin, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  beep(50);
  beep(50);
  beep(50);
  lcd.begin(16, 2); 
  
  
  dht22.begin();
  delay(1000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
  temperature = dht22.readTemperature();
  value = analogRead(AOUTpin);
  limit = digitalRead(DOUTpin);
  Serial.print("Gas rating: ");
  Serial.print(value, 1);
  Serial.print(" Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" ,\n");

  lcd.setCursor(0, 0);
  //lcd.print(millis()/1000);
  lcd.print("Gas rating: ");
  lcd.print(value, 1);
  lcd.setCursor(0,1);
  lcd.print("Temperature: ");
  lcd.print(temperature, 1);
  
  delay(300);
   
  if (value > 200) {
    beep(200);
    if (k == 0)
    {
      k = 1;
      sendSms(GAS_ALERT,200);
    }
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
  }
  else if (temperature > 28) {
    beep(300);
    if (k == 0)
    {
      k = 1;
      sendSms(TEMP_ALERT,300);
    }
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
  }
  else {
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
  }
}
void beep(unsigned char delayms) {
  digitalWrite(buzzer, HIGH);
  delay(delayms);
  digitalWrite(buzzer, LOW);
  delay(delayms);
}
void sendSms(String message,int beepNum){
    digitalWrite(GSM_ON, HIGH);                   // GSM_ON = HIGH 

  // Init  "M95_HardwareDesign_V1.2.pdf", page 30ff.
  // in any case: force a reset!
  digitalWrite(PWRKEY, LOW);            // PWRKEY = HIGH
  digitalWrite(EMERG, HIGH);            // EMERG_OFF = LOW
  delay(50);                                              
  beep(beepNum);
  digitalWrite(PWRKEY, LOW);            // PWRKEY = HIGH
  digitalWrite(EMERG, LOW);  
  // EMERG_OFF = HIGH
  beep(beepNum);
  delay(2100);                                              
  beep(beepNum);
  digitalWrite(PWRKEY, HIGH);           // PWRKEY = LOW
  delay(1100);        
  beep(beepNum);                                      
  digitalWrite(PWRKEY, LOW);            // PWRKEY = HIGH
  beep(beepNum);
  // Start and Autobauding  ("M95_AT_Commands_V1.0.pdf", page 35)
  delay(3000);                                             
  beep(beepNum);
  //---------------------------------------------------------------------------------------------------------------

  
  GSM_string += " first AT:\r\n";
  Serial.print("AT+IPR=0\r");
  //Serial.print("AT\r");                                                
  WaitAndRead(35);                                                    

  beep(beepNum);
  GSM_string += "Set defaults:\r\n";
  Serial.print("AT&F\r");
  //Serial.print("AT&F0\r");                                              
  WaitAndRead(35);                                                     
  beep(beepNum);
  //---------------------------------------------------------------------------------------------------------------

  beep(beepNum);
  GSM_string += "SIM status:\r\n";
  Serial.print("AT+CPIN?\r");                                                
  WaitAndRead(100);
  beep(beepNum);                                                   
  // Eğer SIM kart kilidiniz varsa bu kısmı aktif edin ve kartınız sim kodunu yazın !//
  /*
  GSM_string += "Enter SIM pin:\r\n";
  Serial.print("AT+CPIN=1994\r");                                       // enter pin (SIM)     
  WaitAndRead(100);                                                     
  */
  delay(5000);  
  beep(beepNum);                                                       
 
  //---------------------------------------------------------------------------------------------------------------
  beep(beepNum);
  GSM_string += "GSM network :\r\n";
  Serial.print("AT+CREG?\r");                                           
  WaitAndRead(35);                                                   
  beep(beepNum);
                                                   
  GSM_string += "IMEI:\r\n";
  Serial.print("AT+GSN\r");                                             
  WaitAndRead(35);                                                   
  beep(beepNum);
 
  GSM_string += "RF signal strength:\r\n";
  Serial.print("AT+CSQ\r");                                             
  WaitAndRead(35);                                                     
  beep(beepNum);
  
  GSM_string += "selected operator:\r\n";
  Serial.print("AT+COPS?\r");                                          
  WaitAndRead(35);                                                    
  beep(beepNum);
  //---------------------------------------------------------------------------------------------------------------
 

  Serial.print("AT+CMGF=1\r"); 
  WaitAndRead(35);
  beep(beepNum);
  Serial.print("AT+CSCS=GSM\r");
  WaitAndRead(35);
  beep(beepNum);
  Serial.print("AT+CMGS=\"");
  Serial.print("+900000000000"); //write the phone number
  Serial.print("\"\r");
  delay(1000);
  beep(beepNum);
  Serial.print(message);                                                       // Message-Text
  beep(beepNum);
  Serial.write(26); 
  beep(beepNum);
  delay(5000);
  
  digitalWrite(GSM_ON, LOW);                    // GSM_ON = LOW --> switch off + Serial Line disable
  beep(beepNum);
  Serial.print(GSM_string);
  beep(beepNum);
 // Serial.println("--- E N D ---");
 
 
 delay(5000);
 beep(beepNum);
}
void WaitAndRead(int timeout)
{
  for (int i = 0; i < timeout; i++)
  {
    delay(10);
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      GSM_string += inChar;
    }
  }

  GSM_string += "----------\r\n";
}

