#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
SoftwareSerial arduino(D6,D7);
//ArduinoJson
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

#define BOTtoken "6356213264:AAGdqdboiQabCL-bqmBCo1ncwSU49KqEYEI"
#define CHAT_ID "-4004373256"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
const IPAddress server(0,0,0,0);
const int httpPort = 80;
const int BUFFER_SIZE = JSON_OBJECT_SIZE(2048);
// const char* ssid = "STMKG Spot";
// const char* password = "stmkg2014";
const char* ssid = "BOLT!Super4G-29C2";
const char* password = "eqy04890";

void setup () {
  arduino.begin(9600);
  Serial.begin(9600);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  // WiFi.mode(WIFI_STA);
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
  bot.sendMessage(CHAT_ID, "Wifi Terhubung!", "");
  bot.sendMessage(CHAT_ID, "System dimulai!!", "");
  delay(2000); 
}

void loop () {
  // Cek jika ada pesan yang masuk dari Arduino
  if (arduino.available() > 0) {
    // Membaca data dari Arduino
    String data = arduino.readStringUntil('\n');

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, data);

  
    int level = doc["a"];
    // int hujan = doc["b"];
    Serial.print("Received data: ");
    Serial.println(level);
     if (isnan(level)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
     }
    if ((WiFi.status() == WL_CONNECTED)) {
      if(level < 300 && level >= 250) {
        String message = "Status Banjir : SIAGA\nTinggi air melampaui batas, saat ini tinggi air: " + String(level) + " cm";
        bot.sendMessage(CHAT_ID, message, "");
      }
      if (level < 250 && level >= 200) {
        String message = "Status Banjir : WASPADA\nTinggi air melampaui batas, saat ini tinggi air: " + String(level) + " cm";
        bot.sendMessage(CHAT_ID, message, "");
      }
      if (level < 200){
        String message = "Status Banjir : AWAS\nTinggi air melampaui batas, saat ini tinggi air: " + String(level) + " cm";
        // sendMessage(telegramGroupId, message);
        // Wait 5 seconds before sending another message (adjust as needed)
        bot.sendMessage(CHAT_ID, message, "");
      }
      std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
      client->setInsecure();
      
      //create an HTTPClient instance
      HTTPClient https;
      
      String address;
      //equate with your computer's IP address and your directory application
      // C:\xampp\htdocs\arducoding_tutorial\nodemcu_log\webapi\api\create.php
      address ="https://hanip.my.id/webapi/api/create.php?level=";
      address += String(level);
      address += "&hujan=0"; 
      // address += String(hujan) ;
        
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
  delay(4000);
  
  }
  
}
