// Software Serial (Komunikasi ke ESP8266)
#include <SoftwareSerial.h>
SoftwareSerial arduino(2,3);
const int pwPin = 9;
unsigned long pulseWidth;
unsigned long distance;

//ArduinoJson
#include <ArduinoJson.h>

void setup () {
  Serial.begin(9600);
  pinMode(pwPin, INPUT);
  arduino.begin(9600);
}

void loop () {
    // put your main code here, to run repeatedly:
  pulseWidth = pulseIn(pwPin, HIGH); // Read the pulse width
  distance = pulseWidth / 58; // Convert pulse width to distance in centimeters (adjust the constant according to your sensor specifications)

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  StaticJsonDocument<200> doc;
  doc["a"] = distance;

  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  arduino.println(jsonString);

  delay(2000);
}

