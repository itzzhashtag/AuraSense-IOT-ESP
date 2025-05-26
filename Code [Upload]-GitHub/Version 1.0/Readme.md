
---

## 📄 **2. `README.md` — 🌍 Advanced Weather UI (Tomorrow.io + Full Metrics)**

```markdown
# 🌍 ESP32 Advanced Weather Station (Tomorrow.io + Full Dashboard UI)

A powerful weather dashboard powered by **Tomorrow.io** API and hosted on an ESP32 microcontroller. Displays detailed environmental data with modern UI and API optimization.

![ESP32](https://img.shields.io/badge/Board-ESP32-blue)
![API](https://img.shields.io/badge/API-Tomorrow.io-purple)
![UI](https://img.shields.io/badge/UI-Advanced-darkgreen)

---

## ⚙️ Built With

- 🌐 **Tomorrow.io API (v4)**
- ⚙️ ESP32 Dev Board
- 🖥️ WebServer with dynamic HTML/CSS
- 📡 Asynchronous JSON data fetch

---

## 🔗 Tomorrow.io API - Smart Weather Intelligence

> [🔗 https://www.tomorrow.io](https://www.tomorrow.io)

- Highly granular weather data (down to 1-minute resolution)
- Metrics like:
  - 🌡️ Temperature
  - 💧 Humidity
  - 💨 Wind Speed
  - 🌞 UV Index
  - 🌫️ Visibility
  - 🌡️ Pressure
  - 🌦️ Condition

### Sample API Request:
```http
GET https://api.tomorrow.io/v4/weather/realtime?location=28.6,77.2&apikey=YOUR_API_KEY
