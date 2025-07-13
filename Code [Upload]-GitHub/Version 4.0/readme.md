<div align="center">

# 🌐 AuraSense-IOT-4.0 🚀

**by [Aniket Chowdhury](mailto:micro.aniket@gmail.com) (aka `#Hashtag`)**

<img src="https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge&logo=arduino" alt="Status Badge" />
<img src="https://img.shields.io/badge/Built%20with-Arduino-blue?style=for-the-badge&logo=arduino" alt="Arduino Badge" />
<img src="https://img.shields.io/badge/License-Personal--Use-orange?style=for-the-badge" alt="License Badge" />

</div>

---

<div align="center">

AuraSense IOT (ESP) is a triple-ESP32 based real-time room environment, harmful gases and air quality monitoring system with Dot Matrix display and Web UI.
Now with adaptive thresholds, real-time inter-ESP communication, and intelligent alerting!

</div>

---

## 🚀 What's New in Version 4.0

- 📡 **Reliable Multi-ESP Communication**  
  ESP-3 (MQ5 Client) → ESP-2 (Main Controller) with robust `WiFiClient` transfer, retry-safe.

- 🧠 **Smart Thresholds Based on Temp & Humidity**  
  - Dynamic Safe & Alert thresholds.
  - Based on **environmental context** (non-linear mapping).
  - Always maintain a **~200 unit gap** between Safe and Alert zones.
  - Smooth response even in fluctuating conditions.

- 🌐 **Debug-Friendly Serial Output**
  - Full status: 🌡 Temperature, 💧 Humidity, ✅ Safe Threshold, ⚠️ Alert Threshold, MQ5 Reading.
  - Clear transfer logs with status flags.

- 🔔 **Intelligent Alarm Handling**
  - Adaptive buzzer warning patterns.
  - `Status: Safe`, `Caution`, or `Gas Leaking..!!` with clear indicator LED.

- 🧪 **Field Tested**
  - Live-tested with real air (literally hanging outside the window 😄)
  - Stability across varying weather conditions.


---

## 🖥️ System Architecture


```
[ESP3] →→→ [ESP2] →→→ [ESP1] →→→ Dot Matrix Display + Web UI
|
+-- WebServer (port 80)
```

### Hardware

| Component                          | Qty |
|------------------------------------|-----|
| ESP32 Boards              | 3   |
| MAX7219 Dot Matrix Modules (8x8)    | 16  |
| MQ5 Gas Sensor                      | 1   |
| MQ135 Air Quality Sensor            | 1   |
| MQ7 CO Sensor                       | 1   |
| DHT22 Temperature/Humidity Sensor   | 1   |
| LDR (for ambient light detection)   | 1   |
| 5V Power Supply                     | 1+  |
| Misc. jumpers, connectors, resistors| -   |

### Software / Libraries

- Arduino IDE  
- MD_Parola  
- MD_MAX72xx  
- ArduinoJson  
- WebServer (ESP32 / ESP8266)  
- WiFi / HTTPClient  
- Tomorrow.io Weather API  

---

## ⚙️ Code Configuration

### ESP1 - Display Controller

```cpp
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

IPAddress local_IP(xxx, xxx, xxx, xxx);
IPAddress gateway(xxx, xxx, xxx, xxx);

// Tomorrow.io API
const char* weatherAPI = "https://api.tomorrow.io/v4/weather/realtime?...&apikey=YOUR_API_KEY";

// Display static time
const unsigned long staticTime = 1 * 60 * 1000;
```
### ESP2 - Sensor Aggregator

```cpp
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

const int esp1_port = 83;
const int esp3_port = 82;
```
### ESP3 - Gas Sensor Node

```cpp
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

const char* esp2_ip = "ESP2_STATIC_IP";
const int esp2_port = 81;
```

---

## 📸 Screenshots / Demo (Add Later)

> _Esp-3 Serial Monitor_
<img width="642" height="406" alt="image" src="https://github.com/user-attachments/assets/8a9cd257-02c6-4f73-8b35-4b057ffcd622" />


### 🌐 Web Interface
- Hosted at http://ESP1_IP/ (port 80)
- Responsive live sensor data
- Example UI Screenshot:
![image](https://github.com/user-attachments/assets/f4c7208b-ed97-4ddb-89be-d28776b153b3)


---

### 📡 Communication Ports
Device	Port
- ESP1 Web UI	80
- ESP1 Server	83
- ESP2 Server	81
- ESP3 → ESP2	82

---

### 🛡 Stability & Safety Features
- ✅ Wi-Fi auto-reconnect
- ✅ Failsafe ESP1 restart if no data in 5 min
- ✅ Brightness auto-adjust via LDR
- ✅ Time sync via NTP
- ✅ Manual OTA flashing ready

---

### 🚧 Future Improvements
- Enhanced Web UI with charts 📈
- OTA updates
- Store last data to EEPROM
- More sensors (ESP4...)
- Logging with SD Card

---

<div align="center">
⭐️ If you found this project useful — please consider starring the repo! ⭐️

</div> 

