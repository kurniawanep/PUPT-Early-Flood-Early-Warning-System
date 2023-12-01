#include <Arduino.h>
#include <Wire.h>
#include <UniversalTelegramBot.h>
#include "WiFiS3.h"
#include "WiFiSSLClient.h"
#define BOTtoken "6356213264:AAGdqdboiQabCL-bqmBCo1ncwSU49KqEYEI"
#define CHAT_ID "-4004373256"
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// PANDUAN
// Pin ARG di Digital 3
// Pin Level di Analog (belum dicoding)
// nama variabel tinggi air adalah level sedangkan jarak yang dibaca sensor adalah jarak
// LCD sama RTC di SDA SCL

const int reedSwitchPin = 3; // Digital pin 13
volatile int tipCount10 = 0;    // Counter for tip events
volatile int tipCount24 = 0;    // Counter for tip events
volatile float conversionFactor = 0.2;
float rain10=0;
float rain24=0;
unsigned long lastResetTime10 = 0; // Variable to store the last reset time
unsigned long lastResetTime24 = 0; // Variable to store the last reset time
unsigned long jarak, prevJarak;
int level;

WiFiSSLClient client;
UniversalTelegramBot bot(BOTtoken, client);

void IRAM_ATTR reedSwitchISR() {
  tipCount10++; // Increment the tip count when the reed switch is triggered
  rain10 = tipCount10 * conversionFactor;
  tipCount24++; // Increment the tip count when the reed switch is triggered
  rain24 = tipCount24 * conversionFactor;
}


void setup() {
  Serial.begin(9600); // Initialize serial communication
  char ssid[] = "STMKG Spot";        // your network SSID (name)
  char pass[] = "stmkg2014";    // your network password (use for WPA, or use as key for WEP)
  int keyIndex = 0;                 // your network key index number (needed only for WEP)
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Menghubungkan");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("WiFi..");   // Display a label
  int led =  LED_BUILTIN;
  int status = WL_IDLE_STATUS;
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Tersambung");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print(WiFi.localIP());   // Display a label
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  delay(1000);
  bot.sendMessage(CHAT_ID, "Wifi UNO R4 Connected!", "");
  bot.sendMessage(CHAT_ID, "System has Started!!", "");
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Memulai");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("Sistem");   // Display a label
  delay(1000);
}

void loop() {
  DateTime now = rtc.now();
  unsigned long currentTime = now.unixtime();
  if (currentTime - lastResetTime10 >= 600) {
    // Reset the tipCount to zero
    tipCount10 = 0;
    rain10 = 0; 
    
    // Update the last reset time
    lastResetTime10 = currentTime;
  }
  if (currentTime - lastResetTime24 >= 86400) {
    // Reset the tipCount to zero
    tipCount24 = 0;
    rain24 = 0; 
    
    // Update the last reset time
    lastResetTime24 = currentTime;
  }

  // jarak = ..... (dalam cm)

  level = tinggiSensor - jarak;
  
  Serial.print("Level: ");
  Serial.print(level);
  Serial.println(" cm");

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
      address ="https://rekayasa.stmkg.ac.id/api/data?apiKey=TWDo3y4sAEIxqRsV&curah_hujan=";
      address += String(rain10);
      address += "&curah_hujan_24jam=";
      address += String(rain24);
      address += "&tinggi_air=";
      address += String(level);

        
      https.begin(*client,address);  //Specify request destination
      int httpCode = https.POST("");//Send the request
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
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("CH10: ");   // Display a label
  lcd.print(rain10); 
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("CH24: ");   // Display a label
  lcd.print(rain24); 

  delay(4000);

}


