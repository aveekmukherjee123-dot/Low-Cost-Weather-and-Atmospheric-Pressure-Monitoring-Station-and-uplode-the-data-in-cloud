# Low-Cost Weather & Atmospheric Pressure Monitoring Station

An IoT weather station built on an **ESP32**, a **DHT22** (temperature + humidity) and a **BMP180** (barometric pressure + altitude). Readings are uploaded to the **Blynk IoT Cloud** every few seconds and shown on a live dashboard. The whole project runs in the **Wokwi** simulator, so no hardware is needed, and it ports directly to a real ESP32.

## Features
- Temperature and humidity (DHT22)
- Barometric pressure, sea-level adjusted pressure, and altitude (BMP180)
- Simple on-device weather forecast from pressure level + short-term trend
- Heat index and dew point on the serial monitor
- Cloud dashboard (web + mobile) through Blynk
- Under 5 Blynk datastreams, so it fits the free plan

## Architecture
```
DHT22 ──(1-wire, GPIO15)──┐
                          ├── ESP32 ──WiFi──> Blynk Cloud ──> Web / Mobile Dashboard
BMP180 ──(I2C 21/22)──────┘
```

## Hardware / Wiring
| Part | Pin | ESP32 |
|---|---|---|
| DHT22 | VCC | 3V3 |
| DHT22 | SDA (data) | GPIO 15 |
| DHT22 | GND | GND |
| BMP180 | VCC | 3V3 |
| BMP180 | SDA | GPIO 21 |
| BMP180 | SCL | GPIO 22 |
| BMP180 | GND | GND |

## Blynk setup
1. Sign up at [blynk.cloud](https://blynk.cloud).
2. **Developer Zone > My Templates > New Template**: name `Weather Station`, hardware `ESP32`, connection `WiFi`.
3. In the template, add these **Datastreams** (Virtual Pin):

| Name | Pin | Type | Units | Min | Max |
|---|---|---|---|---|---|
| Temperature | V0 | Double | °C | -10 | 60 |
| Humidity | V1 | Double | % | 0 | 100 |
| Pressure | V2 | Double | hPa | 900 | 1100 |
| Altitude | V3 | Double | m | -100 | 4000 |
| Forecast | V4 | String | | | |

4. **Web Dashboard**: add Gauges for V0, V1, V2, V3, a Label for V4, and a Chart for V0/V1/V2. Save the template.
5. **Devices > New Device > From template**. Open the device, copy `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME` and `BLYNK_AUTH_TOKEN`.
6. Paste them into `config.h`.

## Run in Wokwi
1. Go to [wokwi.com](https://wokwi.com), **New Project > ESP32**.
2. Replace the files with this repo's `sketch.ino`, `config.h`, `libraries.txt` and `diagram.json` (use the **+** tab for new files).
3. Click **Start Simulation**. The serial monitor should show `Connected to Blynk Cloud`.
4. Click the DHT22 and BMP180 in the diagram and move the sliders (temperature, humidity, pressure) and watch the Blynk dashboard update.

> The BMP180 part type in Wokwi is `board-bmp180` (pins: VCC, 3.3V, GND, SCL, SDA). Click the sensors during simulation to change temperature, humidity and pressure.

## Sample serial output
```
T=28.0C  H=65.0%  P(stn)=1013.2 hPa  P(sea)=1013.2 hPa  Alt=0.0 m  HI=31.1C  DP=21.1C  | Fair weather
```

## Forecast logic
| Condition | Result |
|---|---|
| Pressure drops 1.6 hPa or more over the trend window | Rain/storm likely |
| Pressure rises 1.6 hPa or more | Clearing up |
| Otherwise < 1000 / < 1013 / < 1022 / above | Unsettled / Cloudy / Fair / Clear and dry |

## Moving to real hardware
Same code and wiring. In `config.h`, set your real WiFi SSID/password, set `STATION_ALTITUDE_M`, and for real trend detection use a 5-minute interval with `TREND_SAMPLES 36` (3 hours).

## Security
Never commit your real `BLYNK_AUTH_TOKEN` or WiFi password. Keep placeholders in the repo.

## Future Direction
This station is a low-cost starting point, and it can grow in several directions:

- **Better sensing:** add a rain gauge, anemometer (wind speed), UV and light sensors, and an air-quality sensor (PM2.5/CO2) to build a full atmospheric profile. A BME280 can replace the DHT22 + BMP180 pair with one chip.
- **Smarter forecasting:** replace the threshold-based forecast with a small on-device ML model (TinyML) trained on long-term pressure, temperature and humidity trends to predict rain more accurately.
- **Power and deployment:** use deep sleep, a solar panel and a Li-ion battery for off-grid outdoor use, with a weatherproof enclosure and a radiation shield for accurate temperature readings.
- **Connectivity:** add LoRa for areas without WiFi, plus MQTT or ThingSpeak as a backup data path alongside Blynk.
- **Alerts and local display:** Blynk push notifications for storms, extreme heat or rapid pressure drops, and an OLED/e-paper screen for on-site readings.
- **Data and analytics:** log data to a database (e.g. InfluxDB + Grafana) for long-term charts, and pool data from several stations to build a low-cost local weather network for farms, schools or neighbourhoods.

## License
MIT
