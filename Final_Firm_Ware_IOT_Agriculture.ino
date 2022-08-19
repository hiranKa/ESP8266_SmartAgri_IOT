// Use only the 5,4,14,12,13
// Configure local web address need to install the MDNS library in the code
// Need to create two html functions for homepage and the controlling panel
// In the homepage readings from the DHT11 will be showed and the hyperlink will be added
// In the control panel all the toggling of relays will be done 
// need to use two diffrent fucntions for this 

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTTYPE DHT11

int ledPin = 2;    // Built in LED pin
int relayOne = 14;  // Pin Out reference to the D5
int relayTwo = 12;  // Pin Out reference to the D6
int dhtPin  = 13;   // Pin out reference to the D7
int relayThree = 2;
float Temperature;
float Humidity;
bool relayOneStatus, relayTwoStatus, relayThreeStatus; //variable to store the current status of the relays
int thresholdTemp = 35;
int thresholdHum = 65;

ESP8266WebServer server(80);
DHT dht(dhtPin,DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);

char* ssid = "SLT-4G_CHAMEERA";         // Connecting to the WiFi network 
char* password = "sltslt123";   // Enter the WiFi password

//Calling out the functions for the compiler to read before the main fcuntion
void ledIndicator();
void handle_OnConnect();
void toggleOneOn();
void toggleOneOff();
void toggleTwoOn();
void toggleTwoOff();
void toggleThreeOn();
void toggleThreeOff();
String SendHTML(float Temperaturestat,float Humiditystat);
String SendHTMLCP();
void displayR1Stat();
void displayR2Stat();
void displayR3Stat();
//Run setup fucntion for the configurations
void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);
  pinMode(dhtPin, INPUT);
  Serial.println("Connecting to the WiFi Network:");
  lcd.setCursor(0,0);
  lcd.print("Connecting...");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  Serial.println("Connecting to WiFi Network_");
  while (WiFi.status() != WL_CONNECTED) {
    ledIndicator();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SUCCESS");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  // Define the links to pages on the web server 
  server.on("/",handle_OnConnect);
  server.on("/toggleOneOn",toggleOneOn);
  server.on("/toggleOneOff",toggleOneOff);
  server.on("/toggleTwoOn",toggleTwoOn);
  server.on("/toggleTwoOff",toggleTwoOff);
  server.on("/toggleThreeOn",toggleThreeOn);
  server.on("/toggleThreeOff",toggleThreeOff);
  // Start the Web Server
  server.begin();
  Serial.println("HTTP Web Server Started");
  lcd.clear(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
void ledIndicator(){
  digitalWrite(ledPin,!digitalRead(ledPin));
  delay(100);
}

void handle_OnConnect(){
//  digitalWrite(relayOne, LOW);
//  digitalWrite(relayTwo, LOW);
  Temperature = dht.readTemperature();      // get the Temperature readings from DHT Sensor 
  Humidity =  dht.readHumidity();           // get the Humidity readings from DHT sensor
  // Check if any reads failed and exit early (to try again)
  if (isnan(Humidity) || isnan(Temperature)) {        
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  //Compare the Temp and Humidity values and automate the pump operation,
  if(Temperature >= thresholdTemp){
    digitalWrite(relayOne, HIGH);
    relayOneStatus = true; 
  }else{
    digitalWrite(relayOne, LOW);
    relayOneStatus =  false;
  }
  //print the values in the LCD
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(Temperature);
  lcd.setCursor(8,0);
  lcd.print("H:");
  lcd.setCursor(10,0);
  lcd.print(Humidity);  
  // Send the sensor readings to the Serial Monitor for debugging
  Serial.print(F("Humidity: "));
  Serial.print(Humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(Temperature);
  Serial.println(F("°C "));
  server.send(200,"text/html", SendHTML(Temperature,Humidity));     // Send the readings to the Web server to display in the Homepage
}

void toggleOneOn(){
  relayOneStatus = true;
  digitalWrite(relayOne,HIGH);
  server.send(204,"");
  displayR1Stat();
}
void toggleOneOff(){
  relayOneStatus = false;
  digitalWrite(relayOne,LOW);
  server.send(204,"");
  displayR1Stat();
}
void displayR1Stat(){
  if(!relayOneStatus){
    lcd.setCursor(0,1);
    lcd.print("R1:ON"); 
  }else{
    lcd.setCursor(0,1);
    lcd.print("R1:OFF");
  }
}
void toggleTwoOn(){
  relayTwoStatus = true;
  digitalWrite(relayTwo,HIGH);
  server.send(204,"");
  displayR2Stat();
}
void toggleTwoOff(){
  relayTwoStatus = false;
  digitalWrite(relayTwo,LOW);
  server.send(204,"");
  displayR2Stat();
}
void displayR2Stat(){
  if(!relayTwoStatus){
    lcd.setCursor(7,1);
    lcd.print("R2: ON"); 
  }else{
    lcd.setCursor(7,1);
    lcd.print("R2:OFF");
  }  
}
void toggleThreeOn(){
  relayThreeStatus = true;
  digitalWrite(relayThree,HIGH);
  server.send(204,"");
  displayR2Stat();
}
void toggleThreeOff(){
  relayThreeStatus = false;
  digitalWrite(relayThree,LOW);
  server.send(204,"");
  displayR2Stat();
}
void displayR3Stat(){
  if(!relayThreeStatus){
    lcd.setCursor(13,1);
    lcd.print("R3:ON"); 
  }else{
    lcd.setCursor(13,1);
    lcd.print("R3:OFF");
  }
}
String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>EAMS</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;color: #f39c12;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  //Class for displaying the output control buttons
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h2 {color: #444444;margin: 50px auto 50px;} h2 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 20px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,100);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  
   ptr +="<div id=\"webpage\">\n";
   
   ptr +="<h1>SMART AGRICULTURE SYSTEM</h1>\n";
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side temperature-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side temperature-text\">Temperature</div>\n";
   ptr +="<div class=\"side-by-side temperature\">";
   ptr +=(float)Temperaturestat;
   ptr +="<span class=\"superscript\">℃</span></div>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side humidity-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
   ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side humidity-text\">Humidity</div>\n";
   ptr +="<div class=\"side-by-side humidity\">";
   ptr +=(float)Humiditystat;
   ptr +="<span class=\"superscript\">%</span></div>\n";
   ptr +="</div>\n";
   if(relayOneStatus)
  {ptr +="<p>WATER SPRINKLE: ON</p>\n";}
   else
  {ptr +="<p>WATER SPRINKLE: OFF</p>\n";}
  ptr +="<h2><span style='font-size:50px;'>&#128295;</span>OUTPUT CONTROLS</h2>\n";
  if(relayOneStatus)
  {ptr +="<p>WATER SPRINKLE: ON</p><a class=\"button button-off\" href=\"/toggleOneOff\">OFF</a>\n";}
  else
  {ptr +="<p>WATER SPRINKLE: OFF</p><a class=\"button button-on\" href=\"/toggleOneOn\">ON</a>\n";}
  //Button Two
  if(relayTwoStatus)
  {ptr +="<p>SUN LIGHT DIFFUSER: ON</p><a class=\"button button-off\" href=\"/toggleTwoOff\">OFF</a>\n";}
  else
  {ptr +="<p>SUN LIGHT DIFFUSER: OFF</p><a class=\"button button-on\" href=\"/toggleTwoOn\">ON</a>\n";}
  // Button Three
  if(relayThreeStatus)
  {ptr +="<p>FERTILIZER: ON</p><a class=\"button button-off\" href=\"/toggleThreeOff\">OFF</a>\n";}
  else
  {ptr +="<p>FERTILIZER: OFF</p><a class=\"button button-on\" href=\"/toggleThreeOn\">ON</a>\n";}
  //OLD BUTTON CODE
//  ptr +="<p>WATER SPRAY</p><a class=\"button button-on\" href=\"/toggleOne\">TOGGLE</a>\n";
//  ptr +="<p>CURTAIN ON/OFF</p><a class=\"button button-on\" href=\"/toggleTwo\">TOGGLE</a>\n";
//  ptr +="<p>WATERING</p><a class=\"button button-on\" href=\"/toggleThree\">TOGGLE</a>\n";
//  ptr +="<p>FERTILIZER</p><a class=\"button button-on\" href=\"/toggleFour\">TOGGLE</a>\n";

  //ptr +="<p><a href=\"/control\">CONTROL PANEL</a></p>\n" ;
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
