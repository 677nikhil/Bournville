/* Smart Plant Watering System
   Soil + Ultrasonic + DHT11 + Weather API
   Manual + Auto Mode
   ESP8266 NodeMCU
   Relay: ACTIVE HIGH
*/

#define BLYNK_TEMPLATE_ID "TMPL3SBIZWnEn"
#define BLYNK_TEMPLATE_NAME "Smart Plant Watering"
#define BLYNK_AUTH_TOKEN "95mec1mip1LVZx03Z8wNAHgop_k5eXfy"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// ---------------- WIFI ----------------
char ssid[] = "liii";
char pass[] = "niggaswifi";

// ---------------- WEATHER API ----------------
String apiKey = "e1008b83795041076c1e1c4fa8f756af";
float lat = 18.8135;
float lon = 82.7105;

float apiTemp = 0;
float apiHumidity = 0;
float apiRain1h = 0;
String apiCondition = "Clear";

// ---------------- PINS ----------------
#define soilSensor A0
#define waterPump D3
#define TRIG D5
#define ECHO D6
#define DHTPIN D4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ---------------- SETTINGS ----------------
int moistureThreshold = 40;
int tankLowDistance = 15;

// ---------------- CONTROL FLAGS ----------------
bool manualMode = false;
bool manualPump = false;

// =================================================
// BLYNK BUTTONS
// =================================================
BLYNK_WRITE(V7) {   // Manual Pump
  manualPump = param.asInt();
}

BLYNK_WRITE(V8) {   // Auto Mode
  manualMode = param.asInt();
}

// =================================================
// WEATHER FUNCTION
// =================================================
void checkWeather() {

  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient client;
  HTTPClient http;

  String url = "http://api.openweathermap.org/data/2.5/weather?lat=" 
               + String(lat) +
               "&lon=" + String(lon) +
               "&appid=" + apiKey +
               "&units=metric";

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode == 200) {

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getString());

    apiTemp = doc["main"]["temp"] | 0;
    apiHumidity = doc["main"]["humidity"] | 0;
    apiCondition = (const char*)doc["weather"][0]["main"];
    apiRain1h = doc["rain"]["1h"] | 0.0;
  }

  http.end();
}

// =================================================
// MAIN CONTROL
// =================================================
void systemControl() {

  // -------- Soil Moisture --------
  int raw = analogRead(soilSensor);
  int moisture = map(raw, 0, 1024, 100, 0);

  // -------- Ultrasonic --------
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  // -------- DHT --------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT ERROR");
    return;
  }

  bool pumpState = false;

  // ================= MANUAL MODE =================
  if (manualMode == 1) {

    if (manualPump == 1) {
      digitalWrite(waterPump, HIGH);
      pumpState = true;
      Serial.println("MANUAL MODE → Pump ON");
    } else {
      digitalWrite(waterPump, LOW);
      pumpState = false;
      Serial.println("MANUAL MODE → Pump OFF");
    }
  }

  // ================= AUTO MODE =================
  else {

    if (distance > tankLowDistance) {
      digitalWrite(waterPump, LOW);
      pumpState = false;
      Serial.println("Tank LOW → Pump OFF");
    }
    else if (apiRain1h > 0.5 || apiCondition == "Rain") {
      digitalWrite(waterPump, LOW);
      pumpState = false;
      Serial.println("Rain Expected → Pump OFF");
    }
    else if (moisture < moistureThreshold) {
      digitalWrite(waterPump, HIGH);
      pumpState = true;
      Serial.println("Soil Dry → Pump ON");
    }
    else {
      digitalWrite(waterPump, LOW);
      pumpState = false;
      Serial.println("Moisture OK → Pump OFF");
    }
  }

  // -------- BLYNK UPDATE --------
  Blynk.virtualWrite(V0, moisture);
  Blynk.virtualWrite(V2, (float)distance);
  Blynk.virtualWrite(V3, (float)temperature);
  Blynk.virtualWrite(V4, (float)humidity);
  Blynk.virtualWrite(V5, pumpState);
  Blynk.virtualWrite(V6, apiCondition);

  // -------- SERIAL DEBUG --------
  Serial.println("\n========== SYSTEM DATA ==========");
  Serial.print("Moisture: "); Serial.println(moisture);
  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("Temp: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Weather: "); Serial.println(apiCondition);
  Serial.println("=================================");
}

// =================================================
// SETUP
// =================================================
void setup() {

  Serial.begin(115200);

  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, LOW);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  checkWeather();

  timer.setInterval(30000L, checkWeather);
  timer.setInterval(5000L, systemControl);
}

// =================================================
void loop() {
  Blynk.run();
  timer.run();
}