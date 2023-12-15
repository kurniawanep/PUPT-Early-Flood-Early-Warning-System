#include <Arduino.h>
#include <Wire.h>
#include <UniversalTelegramBot.h>
#include "WiFiS3.h"
#include "WiFiSSLClient.h"
#define BOTtoken "6356213264:AAGdqdboiQabCL-bqmBCo1ncwSU49KqEYEI"
#define CHAT_ID "-4004373256"

WiFiSSLClient client;
UniversalTelegramBot bot(BOTtoken, client);


void setup() {
  Serial.begin(9600); // Initialize serial communication
  char ssid[] = "STMKG Spot";        // your network SSID (name)
  char pass[] = "stmkg2014";    // your network password (use for WPA, or use as key for WEP)
  int keyIndex = 0;                 // your network key index number (needed only for WEP)
  
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
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  delay(1000);
  bot.sendMessage(CHAT_ID, "Wifi UNO R4 Connected!", "");
  bot.sendMessage(CHAT_ID, "System has Started!!", "");
}

void loop() {

}


