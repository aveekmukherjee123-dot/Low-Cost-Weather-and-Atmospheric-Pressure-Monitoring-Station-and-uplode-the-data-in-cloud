#pragma once

// ---------- Blynk (copy these 3 from your Blynk template/device) ----------
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "Weather Station"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"   // NEVER commit your real token

// ---------- WiFi (Wokwi's built-in open network) ----------
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// ---------- Pins ----------
#define DHT_PIN 15   // DHT22 data
#define SDA_PIN 21   // BMP180 SDA
#define SCL_PIN 22   // BMP180 SCL

// ---------- Station settings ----------
#define STATION_ALTITUDE_M  0      // your height above sea level (m), used for sea-level pressure
#define SEND_INTERVAL_MS    5000   // how often to read + upload
#define TREND_SAMPLES       12     // trend window = TREND_SAMPLES x SEND_INTERVAL (demo: 1 min).
                                   // For a real station use 3 h, e.g. 5 min interval and 36 samples.
