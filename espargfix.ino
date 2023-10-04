#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

#define BOTtoken "6356213264:AAGdqdboiQabCL-bqmBCo1ncwSU49KqEYEI"
#define CHAT_ID "-4004373256"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to the I2C address of your LCD

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
const IPAddress server(0,0,0,0);
const int httpPort = 80;
// const int BUFFER_SIZE = JSON_OBJECT_SIZE(2048);
// const char* ssid = "STMKG Spot";
// const char* password = "stmkg2014";
const char* ssid = "STMKG Spot";
const char* password = "stmkg2014";

const int reedSwitchPin = D3; // Digital pin 13
volatile int tipCount = 0;    // Counter for tip events
volatile float conversionFactor = 0.2;
float rain=0;
unsigned long lastResetTime = 0; // Variable to store the last reset time
const unsigned long resetInterval = 86400000; // 1 minute in milliseconds

void IRAM_ATTR reedSwitchISR() {
  tipCount++; // Increment the tip count when the reed switch is triggered
  rain = tipCount * conversionFactor;
  
}


void setup () {
  Serial.begin(9600);
  lcd.begin();
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  WiFi.begin(ssid, password);
    int i=0;
    while(WiFi.status() != WL_CONNECTED){ 
      Serial.print(".");
      delay(1000);     
    }
   
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  bot.sendMessage(CHAT_ID, "Wifi ARG Hulu Terhubung!", "");
  bot.sendMessage(CHAT_ID, "System ARG Hulu dimulai!", "");
  pinMode(reedSwitchPin, INPUT_PULLUP); // Enable internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(reedSwitchPin), reedSwitchISR, FALLING); // Attach ISR to the falling edge of the reed switch signal
  rtc.begin();
  lastResetTime = millis();
  delay(2000); 
}

void loop () {

  unsigned long currentTime = millis();
  if (currentTime - lastResetTime >= resetInterval) {
    // Reset the tipCount to zero
    tipCount = 0;
    rain = 0; 
    
    // Update the last reset time
    lastResetTime = currentTime;
  }

 if ((WiFi.status() == WL_CONNECTED)) {
      std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
      client->setInsecure();
      
      //create an HTTPClient instance
      HTTPClient https;
      
      String address;
      //equate with your computer's IP address and your directory application
      // C:\xampp\htdocs\arducoding_tutorial\nodemcu_log\webapi\api\create.php
      // address ="https://hanip.my.id/webapiarghulu/api/create.php?level=0&hujan=";
      // address += String(rain);
      address ="https://hanip.my.id/webapiarghulu/api/create.php?hujan=";
      address += String(rain);
      address += "&level=0"; 
        
      https.begin(*client,address);  //Specify request destination
      int httpCode = https.GET();//Send the request
      String payload;  
      if (httpCode > 0) { //Check the returning code    
          payload = https.getString();   //Get the request response payload
          payload.trim();
          if( payload.length() > 0 ){
            Serial.println(payload + "\n");
          }
      }
      

      https.end();   //Close connection
  }else{
    Serial.print("Not connected to wifi ");Serial.println(ssid);
  }
  lastResetTime = currentTime;
  DateTime now = rtc.now();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("CRH_HJN: ");   // Display a label
  lcd.print(rain); 

  delay(4000);
  
}