#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define BOTtoken "6356213264:AAGdqdboiQabCL-bqmBCo1ncwSU49KqEYEI"
#define CHAT_ID "-4004373256"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to the I2C address of your LCD

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
const IPAddress server(0,0,0,0);
const int httpPort = 80;
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const int reedSwitchPin = D3; // Digital pin 13
volatile int tipCount10 = 0;    // Counter for tip events
volatile int tipCount24 = 0;    // Counter for tip events
volatile float conversionFactor = 0.2;
float rain10=0;
float rain24=0;
unsigned long lastResetTime10 = 0; // Variable to store the last reset time
unsigned long lastResetTime24 = 0; // Variable to store the last reset time
unsigned long ntpUpdateInterval = 3600000; // Update NTP time every 1 hour (in milliseconds)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void IRAM_ATTR reedSwitchISR() {
  tipCount10++; // Increment the tip count when the reed switch is triggered
  rain10 = tipCount10 * conversionFactor;
  tipCount24++; // Increment the tip count when the reed switch is triggered
  rain24 = tipCount24 * conversionFactor;
}

void setup () {
  Serial.begin(9600);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Menghubungkan");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("WiFi..");   // Display a label

  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Tersambung");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print(WiFi.localIP());   // Display a label
  Serial.println(WiFi.localIP());
  Serial.println();
  bot.sendMessage(CHAT_ID, "Wifi ARG Hulu Terhubung!", "");
  bot.sendMessage(CHAT_ID, "System ARG Hulu dimulai!", "");
  pinMode(reedSwitchPin, INPUT_PULLUP); // Enable internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(reedSwitchPin), reedSwitchISR, FALLING); // Attach ISR to the falling edge of the reed switch signal
  timeClient.begin();
  timeClient.update();
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("Memulai");   // Display a label
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("Sistem");   // Display a label
  delay(1000);
}

void loop () {
  timeClient.update();
  unsigned long currentTime = timeClient.getEpochTime();
  
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
    address ="https://rekayasa.stmkg.ac.id/api/data?apiKey=9mr0uKObVFyG0A5z&curah_hujan=";
    address += String(rain10);
    address += "&curah_hujan_24jam=";
    address += String(rain24);

    https.begin(*client, address);  //Specify request destination
    int httpCode = https.POST(""); //Send the request
    String payload;
    if (httpCode > 0) { //Check the returning code
      payload = https.getString();   //Get the request response payload
      payload.trim();
      if (payload.length() > 0) {
        Serial.println(payload + "\n");
      }
    }

    https.end();   //Close connection
  } else {
    Serial.print("Not connected to wifi ");
    Serial.println(ssid);
  }
  lcd.clear();
  lcd.setCursor(0, 0);    // Set the cursor to the second line
  lcd.print("CH10: ");   // Display a label
  lcd.print(rain10);
  lcd.setCursor(0, 1);    // Set the cursor to the second line
  lcd.print("CH24: ");   // Display a label
  lcd.print(rain24);

  delay(10000);
}
