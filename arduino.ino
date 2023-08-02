// Software Serial (Komunikasi ke ESP8266)
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
SoftwareSerial arduino(2,3);
const int pwPin = 9;
unsigned long pulseWidth;
unsigned long distance;
const int reedSwitchPin = 8; // Change this to your actual pin number
RTC_DS3231 rtc;

volatile unsigned long tenMinutesBucketCount = 0;
volatile unsigned long twentyFourHoursBucketCount = 0;
double rainAmountPerTip = 0.2;

unsigned long tenMinutesDuration = 10 * 60; // 10 minutes in seconds
unsigned long twentyFourHoursDuration = 24 * 60 * 60; // 24 hours in seconds

unsigned long tenMinutesStartTime = 0;
unsigned long twentyFourHoursStartTime = 0;

//ArduinoJson
#include <ArduinoJson.h>

void setup () {
  Serial.begin(9600);
  pinMode(pwPin, INPUT);
  arduino.begin(9600);
  
  //ARG
  pinMode(reedSwitchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(reedSwitchPin), countTipping, FALLING);
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  tenMinutesStartTime = rtc.now().unixtime() ;
  twentyFourHoursStartTime = rtc.now().unixtime() ;
}

void loop () {
    // put your main code here, to run repeatedly:
  pulseWidth = pulseIn(pwPin, HIGH); // Read the pulse width
  distance = pulseWidth / 58; // Convert pulse width to distance in centimeters (adjust the constant according to your sensor specifications)
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  //ARG

  DateTime now = rtc.now();

  // Check if 10 minutes have passed
  if (now.unixtime() - tenMinutesStartTime  >= tenMinutesDuration) {
    // Read the rain gauge data and display it for 10 minutes duration
    double totalRainfall10Minutes = tenMinutesBucketCount * rainAmountPerTip;
    Serial.print("Total Rainfall in the last 10 minutes (mm): ");
    Serial.println(totalRainfall10Minutes);
    tenMinutesBucketCount = 0;
    tenMinutesStartTime = now.unixtime();
    // Reset the 10 minutes tipping bucket count
  }

  // Check if 24 hours have passed
  if (now.hour() == 0 && now.minute() == 0 && now.second() == 0) {
    // Read the rain gauge data and display it for 24 hours duration
    double totalRainfall24Hours = twentyFourHoursBucketCount * rainAmountPerTip;
    Serial.print("Total Rainfall in the last 24 hours (mm): ");
    Serial.println(totalRainfall24Hours);
    twentyFourHoursBucketCount = 0; // Reset the 24 hours tipping bucket count
  }

  StaticJsonDocument<200> doc;
  doc["a"] = distance;
  doc["b"] = tenMinutesBucketCount;

  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  arduino.println(jsonString);

  delay(2000);
  
}


void countTipping() {
  tenMinutesBucketCount++;
  twentyFourHoursBucketCount++;
}

