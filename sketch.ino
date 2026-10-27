/*
 * Low-Cost Weather & Atmospheric Pressure Monitoring Station
 * ESP32 + DHT22 + BMP180 -> Blynk IoT Cloud
 *
 * Blynk datastreams (Virtual Pins):
 *   V0  Temperature (°C)       DHT22
 *   V1  Humidity (%)           DHT22
 *   V2  Pressure (hPa)         BMP180 (sea-level adjusted)
 *   V3  Altitude (m)           BMP180
 *   V4  Forecast (string)      computed from pressure + trend
 */
#include "config.h"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>   // works for BMP180

DHT dht(DHT_PIN, DHT22);
Adafruit_BMP085 bmp;
BlynkTimer timer;
bool bmpOk = false;

// pressure history for trend
float history[TREND_SAMPLES];
int histIdx = 0, histCount = 0;

float dewPoint(float t, float h) {
  const float a = 17.62, b = 243.12;
  float g = log(h / 100.0) + (a * t) / (b + t);
  return (b * g) / (a - g);
}

const char* forecastText(float slp, float trend, bool haveTrend) {
  if (haveTrend) {
    if (trend <= -1.6) return "Falling fast: rain/storm likely";
    if (trend >=  1.6) return "Rising: clearing up";
  }
  if (slp < 1000) return "Unsettled, rain likely";
  if (slp < 1013) return "Cloudy / changing";
  if (slp < 1022) return "Fair weather";
  return "Clear and dry";
}

void sendSensorData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("DHT22 read failed, skipping this cycle");
    return;
  }
  if (!bmpOk) {
    Serial.println("BMP180 not found, skipping this cycle");
    return;
  }

  float stationHpa = bmp.readPressure() / 100.0;
  float slp        = bmp.readSealevelPressure(STATION_ALTITUDE_M) / 100.0;
  float alt        = bmp.readAltitude(101325);          // vs standard sea level
  float hi         = dht.computeHeatIndex(t, h, false);
  float dp         = dewPoint(t, h);

  float trend = 0;
  bool haveTrend = (histCount == TREND_SAMPLES);
  if (haveTrend) trend = slp - history[histIdx];        // oldest sample
  history[histIdx] = slp;
  histIdx = (histIdx + 1) % TREND_SAMPLES;
  if (histCount < TREND_SAMPLES) histCount++;

  const char* fc = forecastText(slp, trend, haveTrend);

  Serial.printf("T=%.1fC  H=%.1f%%  P(stn)=%.1f hPa  P(sea)=%.1f hPa  Alt=%.1f m  HI=%.1fC  DP=%.1fC  | %s\n",
                t, h, stationHpa, slp, alt, hi, dp, fc);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, slp);
  Blynk.virtualWrite(V3, alt);
  Blynk.virtualWrite(V4, fc);
}

BLYNK_CONNECTED() {
  Serial.println("Connected to Blynk Cloud");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  dht.begin();
  bmpOk = bmp.begin();
  if (!bmpOk) Serial.println("Could not find BMP180, check wiring!");

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  timer.setInterval(SEND_INTERVAL_MS, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
