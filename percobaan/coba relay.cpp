#include <Arduino.h>
#include <PID_v1.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// // include lib agar tidak error pada kodingan
#include <SD.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#define LED D0 // Led in NodeMCU at pin GPIO16 (D0).
void setup()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT); // LED pin as output.
}
void loop()
{
    digitalWrite(LED, HIGH); // turn the LED off.(Note that LOW is the voltage level but actually
                             // the LED is on; this is because it is acive low on the ESP8266.
    delay(1000);             // wait for 1 second.
    Serial.println("mati");
    digitalWrite(LED, LOW); // turn the LED on.
    delay(1000);            // wait for 1 second.
    Serial.println("hidup");
}