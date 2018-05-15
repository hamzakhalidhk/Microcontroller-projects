#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>      // Libraries
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
int LDR = 0;
LiquidCrystal lcd(12, 9, 6, 4, 3, 2);


#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(10,11);      // arduino RX pin=10  arduino TX pin=11    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin


#include <DHT.h>
#define DHTPIN 5                // Connect the signal pin of DHT11 sensor to digital pin 5
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


String apiKey = "XN4MD3DYJHUDG8DV";     //Channel's thingspeak WRITE API key
String ssid="Hamza";    // Wifi network SSID
String password ="123pak123";  // Wifi network password
boolean DEBUG=true;

//======================================================================== showResponse
void showResponse(intX waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//================================================================================
  boolean thingSpeakWrite(float value1, float value2, float value3, float value4){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr +="&field4=";
  getStr += String(value4);
  // ...
  getStr += "\r\n\r\n\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    if (DEBUG)  {
      Serial.print(getStr);
      showResponse(1000);
    }
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}
//================================================================================ setup
void setup() {  
  lcd.begin(16, 2);
  pinMode(LDR, INPUT);               
  DEBUG=true;           // enable debug serial
  Serial.begin(9600); 
  dht.begin();          // Start DHT sensor
  
  espSerial.begin(115200);  // enable software serial
                          // Your esp8266 module's speed is probably at 115200. 
                          // For this reason the first time set the speed to 115200 or to your esp8266 configured speed 
                          // and upload. Then change to 9600 and upload again
  
  //espSerial.println("AT+CIOBAUD=9600");         // set esp8266 serial speed to 9600 bps
  //espSerial.println("AT+UART_CUR=9600,8,1,0,0");         // set esp8266 serial speed to 9600 bps
  
  showResponse(1000);
  
  espSerial.println("AT+RST");         // reset esp8266
  showResponse(1000);

  espSerial.println("AT+CWMODE=3");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}


// ====================================================================== loop
void loop() {

  // Read sensor values
   float p= bmp.readPressure();
   float v = analogRead(LDR);
   float t = dht.readTemperature();
   float h = dht.readHumidity();
        if (isnan(t) || isnan(h)) {
        if (DEBUG) Serial.println("Failed to read from DHT");
      }
      else {
          if (DEBUG)  Serial.println("Temp="+String(t)+" *C");
          if (DEBUG) Serial.println("Humidity="+String(h)+" %");
          if (DEBUG) Serial.println("Light_intensity="+String(v)+"");
          if (DEBUG) Serial.println("pressure="+String(p)+"Pa");
           thingSpeakWrite(t,h,v,p);  
           // Write values to thingspeak
      }
  
    
      
   
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(dht.readTemperature());
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(dht.readHumidity());
  lcd.print("%");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Pressure: ");
  lcd.print(bmp.readPressure());
  lcd.print(" Pa");
  lcd.setCursor(0,1);
  
  if(v<100){ lcd.print("Night");}
  else{ lcd.print("Day");}

  // thingspeak needs 3 sec delay between updates, 
  delay(3000);  
}



