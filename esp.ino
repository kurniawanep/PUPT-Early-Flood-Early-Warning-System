#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
SoftwareSerial arduino(D6,D7);
//ArduinoJson
#include <ArduinoJson.h>

WiFiClient client;
const IPAddress server(0,0,0,0);
const int httpPort = 80;
const int BUFFER_SIZE = JSON_OBJECT_SIZE(2048);
const char* ssid = "Dosen Spot";
const char* password = "stmkgcollege";

void setup () {
  arduino.begin(9600);
  Serial.begin(9600);
   WiFi.mode(WIFI_STA);
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
    int hujan = doc["b"];
    Serial.print("Received data: ");
    Serial.println(level);
     if (isnan(level) || isnan(hujan)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
     }
    if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    
    String address;
    //equate with your computer's IP address and your directory application
    // C:\xampp\htdocs\arducoding_tutorial\nodemcu_log\webapi\api\create.php
    address ="http://10.53.0.162/banjir/webapi/api/create.php?level=";
    address += String(level);
    address += "&hujan="; 
    address += String(hujan) ;
      
    http.begin(client,address);  //Specify request destination
    int httpCode = http.GET();//Send the request
    String payload;  
    if (httpCode > 0) { //Check the returning code    
        payload = http.getString();   //Get the request response payload
        payload.trim();
        if( payload.length() > 0 ){
           Serial.println(payload + "\n");
        }
    }
    
    http.end();   //Close connection
  }else{
    Serial.print("Not connected to wifi ");Serial.println(ssid);
  }
  delay(2000);

  }
  
}