#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// include lib agar tidak error pada kodingan
#include <SD.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>

// include sensor dht dan pid
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PID_v1.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Konfigurasi WiFi
const char *ssid = "Note";
const char *password = "12341234";

// URL endpoint untuk mengirim data
const char *endpoint = "http://mastekidmonitor.000webhostapp.com/input.php";

// inisialisasi Dht
#define DHTPIN 2      // D4 pada nodemcu
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);

// Pin trig dan echo pada sensor ultrasonik
const int trigPin = D5;
const int echoPin = D6;

// Pin yang digunakan untuk mengendalikan relay
#define PIN_OUTPUT 13 // D7 pada nodemcu

// variabel2 untuk pid
double Setpoint, Input, Output;
double Kp = 50, Ki = 0.30, Kd = 0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Jarak batas masukkan sampah dan sampah penuh (dalam cm)
const int masukkanSampahThreshold = 8;
const int sampahPenuhThreshold = 7;

// Nilai suhu batas atas dan batas bawah
const int upperThreshold = 75;

// Waktu (dalam milidetik) untuk timer 5 menit
const unsigned long timerDuration = 5 * 60 * 1000;

// Variabel untuk menyimpan waktu awal ketika relay dihidupkan
unsigned long relayStartTime = 0;

void setup()
{
    lcd.init(); // initialize the lcd
    lcd.backlight();
    // Mengatur pin trig sebagai OUTPUT dan echo sebagai INPUT
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(PIN_OUTPUT, OUTPUT);

    // set point suhu
    Setpoint = 60;
    myPID.SetMode(AUTOMATIC);

    // mulai dht
    dht.begin();
    sensor_t sensor;

    // Menghubungkan ke jaringan WiFi
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Menghubungkan ke WiFi...");
    }
    Serial.println("Terhubung ke WiFi!");

    // matikan relay saat awal program untuk keamanan
    digitalWrite(PIN_OUTPUT, LOW);

    // Menunda selama 2 detik sebelum membaca jarak pertama kali
    delay(2000);
}

int readDistance()
{
    // Mengirimkan sinyal ultrasonik
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Menerima waktu pantulan ultrasonik
    long duration = pulseIn(echoPin, HIGH);

    // Menghitung jarak berdasarkan waktu pantulan
    int distance = duration * 0.034 / 2;

    Serial.print("jarak: ");
    Serial.print(distance);
    Serial.println(" cm");

    return distance;
}

float readTemperature()
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println(F("Error reading temperature!"));
    }
    else
    {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
    }

    // Menghitung suhu dalam derajat Celsius
    float temperature = event.temperature;
    // masukkan nilai suhu untuk diproses di library pid
    Input = event.temperature;

    return temperature;
}
void sendSensorData(float suhu, int jarak)
{
    WiFiClient client;
    HTTPClient http;
    String Link, data = "suhu=" + String(suhu) + "&kapasitas=" + String(jarak);
    Link = "http://atmasz.000webhostapp.com/input.php?" + data;

    http.begin(client, Link);

    int httpCode = http.GET();
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
    delay(100);
}

void loop()
{
    // Membaca jarak dari sensor ultrasonik
    int distance = readDistance();
    // Membaca suhu dari sensor LM35
    float suhu = readTemperature();
    // mulai komputasi pid
    myPID.Compute();

    // Mengeluarkan output berdasarkan jarak yang terdeteksi
    if (distance >= masukkanSampahThreshold)
    {
        Serial.println("Masukkan Sampah");
        digitalWrite(PIN_OUTPUT, LOW);
        Serial.println("Relay off");
    }
    else if (distance <= sampahPenuhThreshold)
    {
        Serial.println("Sampah Penuh");
        // digitalWrite(PIN_OUTPUT, HIGH);
        // Serial.println("Relay on");
        // Mengatur status relay berdasarkan suhu yang terdeteksi
        // if (suhu > upperThreshold)
        // {
        //     digitalWrite(relayPin, HIGH);
        //     relayStartTime = millis();
        //     Serial.println("Relay off");
        //     // Mengirim data suhu dan jarak ke website
        // }
        if (Output >= 112.5)
        {
            digitalWrite(PIN_OUTPUT, HIGH);
            Serial.println("relay high");
        }
        else
        {
            digitalWrite(PIN_OUTPUT, LOW);
            Serial.println("relay low");
        }
    }
    lcd.setCursor(0, 0);
    lcd.print(suhu);
    lcd.print("*C ");
    lcd.print("PID: ");
    lcd.print(Output);
    lcd.print("      ");
    lcd.setCursor(0, 1);
    lcd.print("%masker: ");
    lcd.print(distance);
    lcd.print("cm     ");

    // kirim data ke website
    sendSensorData(suhu, distance);
    Serial.print(Output);

    // Menunda sebelum membaca jarak dan suhu berikutnya
    delay(1000);
}
