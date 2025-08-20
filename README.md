<div align="center">
 
# 🌐 AuraSense-IOT-ESP 🔧

**by [Aniket Chowdhury](mailto:micro.aniket@gmail.com) (aka `#Hashtag`)**

<img src="https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge&logo=arduino" alt="Status Badge" />
<img src="https://img.shields.io/badge/Built%20with-Arduino-blue?style=for-the-badge&logo=arduino" alt="Arduino Badge" />
<img src="https://img.shields.io/badge/License-Personal--Use-orange?style=for-the-badge" alt="License Badge" />

</div>

---

<div align="center">
AuraSense IOT (ESP) is a Multi-ESP32 based real-time Home Environment, harmful gases and air quality monitoring system with advanced wireless TCP/IP communication between ESP32 devices, multi-sensor data collection, and dynamic visual outputs on both LED matrix and LCD displays.
</div>

---

## 📡 Project Overview

This project builds an intelligent room air-quality dashboard using Many ESP32 microcontrollers:

- **Client ESP3:** Collects Lpg Gas value from MQ5 and DHT11 Sensor Locally and displays it locally on an LCD display with led indicators and sends the data to Client ESP2 over Wi-Fi.
- **Server ESP2:** Collects air quality, temperature, and humidity data, displays it locally on an LCD display with RGB indicators and sends the data to Client ESP1 over Wi-Fi.
- **Client ESP3:** Receives room environment data and external MQ5 sensor data Network connected ESP1, displays information on a dual 8x32 LED matrix, fetches weather data from Weather API, and hosts a real-time web interface.
---

⚠️ **Important Notice Before Using the Code**

Please make sure to **change the placeholder values and data variables** in the code before running it.

For your convenience, I have added comments indicating the exact **line numbers where changes are needed**.  
 
---

## 🔧 Features

- 📶 **ESP-to-ESP TCP Communication** on a local network.
- 🌡️ **DHT22 Sensor** for real-time temperature and humidity measurement.
- 🌡️ **DHT11 Sensor** for real-time temperature and humidity measurement.
- 🌫️ **MQ-135 Sensor** for indoor air quality monitoring (AQI).
- 🚬 **MQ-7 Sensor** for carbon monoxide (CO) level detection.
- 🍳 **MQ-5 Sensor** for gas leakage monitoring (data fetched from 3rd ESP over TCP).
- 🔳 **LED Matrix Display (2 x 8x32)** on Client ESP for weather + room data.
- 📺 **20x4 LCD Display** on Server ESP for local live display of AQI, CO, temperature, and humidity.
- 🎨 **RGB LED Indicators** to show air quality status (safe/moderate/hazardous).
- 🌍 **Weather Data** integration from [Tomorrow.io](https://www.tomorrow.io/) API (customizable).
- 🕒 **NTP Time Synchronization** for accurate time display and logging.
- 🌐 **Web Server UI** for room and weather data visualization in a browser.
- ♻️ **Auto-Reconnect & Self-Healing Communication** for robustness.

---

## 🖥️ Hardware Used

| Component         | Description                                    |
|------------------|-------------------------------------------------|
| ESP32 (x2)       | Dual-core Wi-Fi + Bluetooth microcontroller     |
| ESP32 (x1)       | Third ESP for remote MQ5 sensor                 |
| DHT22            | Temperature and Humidity Sensor                 |
| DHT11            | Temperature and Humidity Sensor                 |
| MQ-135           | Air Quality Sensor (AQI)                        |
| MQ-7             | Carbon Monoxide Sensor                          |
| MQ-5             | Gas Leak Sensor (data sent wirelessly)          |
| 8x32 LED Matrix  | Dot Matrix Display x 2 Modules                  |
| 20x4 LCD         | I2C LCD Display for room environment readout    |
| RGB LED          | Status Indicator (R-G-B common cathode)         |
| Power Supply     | 5V DC supply                                    |
| Wi-Fi Router     | For ESP-to-ESP communication                    |
| LDR Sensor       | LDR manages the Brightness of the MAtrix display |

---

## 🛠️ Libraries Used

- `WiFi.h`
- `WiFiClient.h`
- `ESPAsyncWebServer.h`
- `DHT.h`
- `Adafruit_Sensor.h`
- `MD_Parola`
- `MD_MAX72XX`
- `LiquidCrystal_I2C`
- `NTPClient`
- `WiFiUdp`
- `HTTPClient`

---

## 📝 Setup Instructions

1. Flash `ESP1.ino` on one ESP1 (Client ESP), `ESP2.ino` on the second ESP32 (Server ESP) and `ESP3.ino` on the third ESP32 (Client ESP).
2. Update Wi-Fi credentials and Tomorrow.io API key in both Server and Client code. 
3. Connect sensors to the correct GPIO pins as described in the code comments.
4. Power all devices and ensure they connect to the same Wi-Fi network.
5. Update Data Values accoring to the Values of the Sensor (Experiment).
6. Access Client ESP's IP in your browser to see live room data and weather.

---

## 📊 Display Layout

### Server ESP (20x4 LCD) - ESP3

| Line | Display Contents                        |
|------|-----------------------------------------|
| 1    | MQ5 Gas Sensor Value (from remote ESP3) |
| 2    | Calibrates Threshold Value Based on DHT11 |
| 3    | Display Appropriate Data on local LCD |
| 4    | Send MQ5 Data to ESP2 over Network |

### Server ESP (20x4 LCD) - ESP2

| Line | Display Contents                        |
|------|-----------------------------------------|
| 1    | Air Quality (ppm value & text)          |
| 2    | CO level (ppm value & text)             |
| 3    | Room Temperature & Humidity             |

### Client ESP (LED Matrix) - ESP1

| Line | Display Contents                        |
|------|-----------------------------------------|
| Top Row  | Dynamic scrolling Weather Data |
| Bottom Row | Air Quality, CO, Temperature, Humidity |

---

## 👤 Author & Contact

👨 **Name:** Aniket Chowdhury (aka Hashtag)  
📧 **Email:** [micro.aniket@gmail.com](mailto:micro.aniket@gmail.com)  
💼 **LinkedIn:** [itzz-hashtag](https://www.linkedin.com/in/itzz-hashtag/)  
🐙 **GitHub:** [itzzhashtag](https://github.com/itzzhashtag)  
📸 **Instagram:** [@itzz_hashtag](https://instagram.com/itzz_hashtag)

---

## 📌 Future Improvements

- Implement full **MQTT** support for scalable IoT cloud communication.
- Add **historical data logging** and cloud analytics.
- Develop a **mobile app** for real-time control and notifications.
- Expand sensor suite with **light**, **noise**, **motion**, and **CO2** sensors.
- Improve Web UI with **dynamic charts**, **graphs**, and **logging**.
- Implement **OTA updates** for easy firmware upgrades.

---

## 📜 License

This project is released under a Modified MIT License.
It is intended for personal and non-commercial use only.

🚫 Commercial use or distribution for profit is not permitted without prior written permission.
🤝 For collaboration, reuse, or licensing inquiries, please contact the author.

📄 View Full License <br>
[![License: MIT–NC](https://img.shields.io/badge/license-MIT--NC-blue.svg)](./LICENSE)

---

## ❤️ Acknowledgements

This is a solo passion project, built with countless nights of tinkering, testing, and debugging.  
If you find it useful or inspiring, feel free to ⭐ the repository or connect with me on social media!

---

> _“If the mind can create, the hands can translate.”_ – Hashtag
