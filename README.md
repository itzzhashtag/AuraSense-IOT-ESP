# AuraSense-IOT-ESP-1 🌐🔧

**AuraSense IOT (ESP-1)** is a dual-ESP32 based real-time room environment monitoring and weather display system. It demonstrates wireless TCP/IP communication between two ESP32 devices, sensor data collection, API integration, and visual output on an LED matrix display.

---

## 📡 Project Overview

This project creates a smart room dashboard using two ESP32 microcontrollers:

- **Server ESP32:** Collects real-time sensor data (Temperature, Humidity, Air Quality) and transmits it via TCP over Wi-Fi.
- **Client ESP32:** Receives sensor data, displays it on a dual 8x32 LED matrix, fetches weather updates from Weather API, and hosts a simple web interface for real-time monitoring.

---

⚠️ **Important Notice Before Using the Code**

Please make sure to **change the placeholder values and data variables** in the code before running it.

For your convenience, I have added comments indicating the exact **line numbers where changes are needed**.  
 
---

## 🔧 Features

- 📶 **ESP-to-ESP TCP Communication** on local network.
- 🌡️ **DHT22** for temperature and humidity sensing.
- 🌫️ **MQ-135** for air quality monitoring.
- 🔲 **LED Matrix Display (2 x 8x32)** using MD_MAX72XX and Parola libraries.
- 🌍 **Weather Data** fetched from a Weather API. [Lot of Options] - I personally Chose www.tomorrow.io
- 🕒 **NTP Time Sync** for accurate data logging.
- 🌐 **Web Server UI** for real-time monitoring via browser.
- ♻️ **Auto-Reconnect & Watchdog Timer** for system reliability.

---

## 🖥️ Hardware Used

| Component         | Description                                 |
|------------------|---------------------------------------------|
| ESP32 (x2)       | Dual-core Wi-Fi + Bluetooth microcontroller |
| DHT22            | Temperature and Humidity Sensor             |
| MQ-135           | Air Quality Sensor                          |
| 8x32 LED Matrix  | Dot Matrix Display x 2 Modules              |
| Power Supply     | 5V DC supply                                |
| Wi-Fi Router     | For ESP-to-ESP communication                |

---

## 🛠️ Libraries Used

- `WiFi.h`
- `WiFiClient.h`
- `ESPAsyncWebServer.h`
- `DHT.h`
- `Adafruit_Sensor.h`
- `MD_Parola`
- `MD_MAX72XX`
- `NTPClient`
- `WiFiUdp`
- `HTTPClient`

---

## 📝 Setup Instructions

1. Flash `server.ino` on one ESP32 and `client.ino` on the other.
2. Update Wi-Fi credentials and OpenWeatherMap API key in the client code.
3. Ensure both devices are powered and connected to the same Wi-Fi.
4. Access the client’s IP in your browser to see real-time data.

---

## 👤 Author & Contact

**Name:** Aniket Chowdhury (aka Hashtag)  
**Email:** micro.aniket@example.com  
**GitHub:** [itzzhashtag](https://github.com/itzzhashtag)  
**Instagram:** [@itzz_hashtag](https://instagram.com/itzz_hashtag)  
**LinkedIn:** [itzz-hashtag](https://www.linkedin.com/in/itzz-hashtag/)

---

## 📌 Future Improvements

- MQTT integration for scalable cloud communication.
- Historical data logging and analytics on cloud.
- Mobile app for remote control.
- More sensors (e.g., CO, light, noise).
- UI improvements on the web server (charts/graphs).

---

## 📜 License

This project is open-source and available under the MIT License.
