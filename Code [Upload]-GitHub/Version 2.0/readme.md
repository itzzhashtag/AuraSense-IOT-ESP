# 🌤️ ESP32 Advanced Weather Station (Tomorrow.io - Rich UI)

An upgraded version of the original weather display project using Tomorrow.io API for a richer weather dataset and a more advanced web interface.

## ✨ Features

- Real-time weather information with detailed metrics
- Responsive and informative HTML UI
- Uses Tomorrow.io API for accurate and granular weather data
- Displays:
  - Temperature
  - Humidity
  - Wind Speed
  - UV Index
  - Pressure
  - Visibility
  - Weather condition summary

## 📦 Components Used

- ESP32 Dev Board
- Wi-Fi connection
- Tomorrow.io API Key

## 🧠 How It Works

1. ESP32 connects to Wi-Fi.
2. Queries Tomorrow.io REST API.
3. Parses JSON data and extracts:
   - Temperature (°C)
   - Humidity (%)
   - Wind speed (km/h)
   - UV index
   - Atmospheric pressure (hPa)
   - Visibility (km)
   - Condition description (e.g., Rain, Clear)

## 🌐 UI Preview

The web interface displays data like:

![image](https://github.com/user-attachments/assets/55e64843-6f5a-491a-93c7-155295dbd080)
