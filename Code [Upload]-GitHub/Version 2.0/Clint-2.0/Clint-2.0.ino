/*
  Hello This is me Hashtag .....
  This is my Personal Home Automation Work, I use this project in my room for sensing my Room Enviroment and receive data from other ESP.
  Then that data is displayed to Dot Matrix with few basic Animations.
  This Code also makes sure the Client & Wifi stays connected or retry...!
  This Also has the Web based UI to show data over Web.. The Ip will be given to you..!
  Name: Aniket Chowdhury [Hashtag]
  Email: micro.aniket@gmail.com
  GitHub: https://github.com/itzzhashtag
  Instagram: https://instagram.com/itzz_hashtag
  LinkedIn: https://www.linkedin.com/in/itzz-hashtag/
*/

//Open File->Preferences->Additional Boards Manager URL's ->(Copy and Paste) "https://dl.espressif.com/dl/package_esp32_index.json,http://arduino.esp8266.com/stable/package_esp8266com_index.json"
//ALso Change Values and Data before using the code (Changes needed) Line Number ->34,35,36,38,39,313,314,315          [Here www.tomorrow.io is being Used]

//===================================================================================================================
// --- Libraries Used ---
//===================================================================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include <time.h>
#include <WebServer.h>
#include <ArduinoJson.h>
//===================================================================================================================
// --- Wi-Fi and Server Constants ---
//===================================================================================================================

const char* ssid = "your_ssid";
const char* password = "ssid_password";
const char* serverIP = "your Wifi/Server Ip"; // same as Server
const uint16_t serverPort = 80;
IPAddress local_IP("Your_local_Ip");
IPAddress gateway("Your_static_Ip");
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // Google DNS
IPAddress secondaryDNS(8, 8, 4, 4); // Backup DNS
WiFiClient client;
WebServer server(80);  // <<<<<<<<<< Web UI Server
//===================================================================================================================
// --- Matrix Pins ---
//===================================================================================================================

#define MAX_DEVICES 8
#define DATA_PIN   23
#define CLK_PIN    18
#define CS_PIN_TOP    5
#define CS_PIN_BOTTOM 4
MD_Parola m2(MD_MAX72XX::FC16_HW, CS_PIN_TOP, MAX_DEVICES);    // Bottom row
MD_Parola m1(MD_MAX72XX::FC16_HW, CS_PIN_BOTTOM, MAX_DEVICES); // Top row
//===================================================================================================================
// --- Global State Variables ---
//===================================================================================================================

int temp = 0, hum = 0, aoq = 0;
String weather_rp = "Loading weather...";
unsigned long previousMillis = 0;
const long interval = 3000;
unsigned long staticStartTime = 0;
const unsigned long staticTime = 1 * 60 * 1000;// (minutes * Seconds * milliseconds) to show the static text before switching back to scrolling
int scrollCount = 0, sc = 5; //scrollcount for the times the top matrix displays the weather report and stuff, sc for max no of scrolls.
bool showingStatic = false;
unsigned long lastWeatherCheck = 0;
const unsigned long weatherInterval = 4 * 60 * 1000; ////minutes(the main changer) * Seconds * milliseconds to update weather report
unsigned long lastDataTime = 0;
const unsigned long timeoutMs = 5000;
//===================================================================================================================
// --- Setup ---
//===================================================================================================================

void setup()
{
  Serial.begin(115200);
  SPI.begin(CLK_PIN, -1, DATA_PIN);
  m1.begin(); m2.begin();
  m1.setIntensity(3);
  m2.setIntensity(3);
  m1.displayClear();
  m2.displayClear();
  connectToWiFi();
  syncTime();           //syncronise time with the Esp module.
  internettest();       //Test the Internet is available or not.
  connectToServer();
  setupWebUI();
  weather_rp = weather_report();
  Serial.println("================================================================================================");
  Serial.println("Weather Report : " + weather_rp);
  Serial.println("================================================================================================");
}
//===================================================================================================================
// --- Loop ---
//===================================================================================================================

void loop()
{
  unsigned long currentMillis = millis();
  // Handle Wi-Fi Reconnection
  if (WiFi.status() != WL_CONNECTED)
  {
    reconnectWiFi();
    lastDataTime = millis();
  }
  // Handle Server Reconnection
  if (!client.connected())
  {
    Serial.println("🚨 Server disconnected! Reconnecting...");
    client.stop();
    connectToServer();
    lastDataTime = millis();
    return;
  }
  // Check for Data from Server
  if (client.available() >= 3 * sizeof(int))
  {
    if (!readSensorData())
    {
      client.stop();  // If read failed
    }
    lastDataTime = millis();
  }
  // Timeout Check
  if (millis() - lastDataTime > timeoutMs)
  {
    Serial.println("❌ No data received. Reconnecting...");
    client.stop();
    connectToServer();
    lastDataTime = millis();
  }
  // Fetch Weather Periodically
  if (millis() - lastWeatherCheck > weatherInterval)
  {
    weather_rp = weather_report();
    lastWeatherCheck = millis();
    Serial.println("🔁 Weather updated:");
    Serial.println(weather_rp);
  }
  // === Matrix 1 (Top) ===
  if (!showingStatic)
  {
    if (m1.displayAnimate())
    {
      m1.displayReset();
      scrollCount++;
      if (scrollCount >= sc)
      {
        showingStatic = true;
        staticStartTime = currentMillis;
        m1.displayClear();
        m1.displayText("Temp Hum AoQ", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
        m1.displayReset();
        m1.displayAnimate();
      }
    }
  }
  else
  {
    if (currentMillis - staticStartTime >= staticTime)
    {
      showingStatic = false;
      scrollCount = 0;
      m1.displayClear();
      weather_rp = fixUTF8(weather_rp); // ensure ASCII-friendly display
      m1.displayText(weather_rp.c_str(), PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      Serial.println(weather_rp);
      m1.displayReset();
    }
  }
  // === Matrix 2 (Bottom) ===
  static bool canUpdateM2 = true;
  if (!showingStatic && !m1.displayAnimate()) canUpdateM2 = true;

  if ((canUpdateM2 || showingStatic) && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d\xB0""C %d%% %d", temp, hum, aoq);
    m2.displayClear();
    m2.displayText(buffer, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    m2.displayReset();
    if (!showingStatic) canUpdateM2 = false;
  }
  // === Watchdog Restart (if no data for 5 minutes) ===
  if (millis() - lastDataTime > 300000)
  {
    Serial.println("🛑 No sensor data for 5 minutes! Restarting ESP...");
    ESP.restart();
  }
  m2.displayAnimate();  // Animate m2
  server.handleClient();// Web UI
}
//===================================================================================================================
// --- Networking Functions ---
//===================================================================================================================

void connectToWiFi()
{
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(ssid, password);
  Serial.print("🔌 Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected!");
}
void reconnectWiFi()
{
  Serial.println("❌ Wi-Fi lost! Reconnecting...");
  WiFi.disconnect(); WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Reconnected to Wi-Fi!");
}
void connectToServer()
{
  Serial.print("🔄 Connecting to server");
  m1.displayClear();
  m1.displayText("Connecting to Server...", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  m1.displayReset();
  while (!client.connect(serverIP, serverPort))
  {
    m1.displayAnimate();  // Animate the scrolling text
    delay(50);            // Small delay for smooth animation
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to server!");
  m1.displayClear();
  m1.displayText(weather_rp.c_str(), PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  m1.displayReset();
}
//===================================================================================================================
// --- Time Sync and Internet Check ---
//===================================================================================================================

void syncTime()
{
  configTime(0, 0, "time.google.com");
  Serial.print("⏳ Waiting for NTP time sync");
  time_t now = time(nullptr);
  int retries = 0;
  while (now < 1000000000 && retries < 20)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    retries++;
  }
  if (now < 1000000000)
  {
    Serial.println("\n❌ Failed to sync time.");
  }
  else
  {
    Serial.println("\n✅ Time synced successfully.");
    // Format and print the synced time
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);  // Use gmtime_r or localtime_r for thread safety
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
    Serial.print("📅 Current Time: ");
    Serial.println(timeString);
  }
}
void internettest()
{
  Serial.println("🌐 Testing internet connection...");
  WiFiClient testClient;
  if (testClient.connect("example.com", 80)) {
    Serial.println("✅ Internet is working!");
    testClient.stop();
  } else {
    Serial.println("❌ No internet access!");
  }
}
//===================================================================================================================
// --- Take in Data from another ESP ---
//===================================================================================================================

bool readSensorData()
{
  if (client.read((uint8_t*)&temp, sizeof(temp)) != sizeof(temp)) return false;
  if (client.read((uint8_t*)&hum, sizeof(hum)) != sizeof(hum)) return false;
  if (client.read((uint8_t*)&aoq, sizeof(aoq)) != sizeof(aoq)) return false;
  Serial.printf("🌡 Temp: %d°C | 💧 Humidity: %d%% | 🌫 AQI: %d\n", temp, hum, aoq);
  return true;
}
//===================================================================================================================
// --- Weather ---
//===================================================================================================================

float cityTemp = 0;
float cityFeelsLike = 0;
float cityMax = 0;
float cityMin = 0;
String cityTimeOfDay = "Unknown";
String cityCondition = "Unknown";
float cityHumidity = 0;
float cityWind = 0;
String cityWindDir = "Unknown";
float cityPressure = 0;
float cityVisibility = 0;
float cityUV = 0;
String weather_report()
{
  const char* city = "Your_City";// City Name or latitude & Longtitude
  const char* apiKey = "your_api_key"; //your APi Key
  String url = "https://api.tomorrow.io/v4/weather/realtime?location=" + String(city) + "&apikey=" + String(apiKey);
  //https://api.tomorrow.io
  //https://api.openweathermap.org
  //https://api.weatherstack.com
  //https://weather.visualcrossing.com
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  HTTPClient https;
  https.begin(secureClient, url);
  int httpCode = https.GET();
  if (httpCode > 0)
  {
    String payload = https.getString();
    https.end();

    const size_t capacity = 2048;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print("❌ JSON Parse Error: ");
      Serial.println(error.c_str());
      return "Error parsing weather data";
    }
    JsonObject values = doc["data"]["values"];

    // Assign values to globals
    cityTemp = values["temperature"];
    cityFeelsLike = values["temperatureApparent"];
    cityHumidity = values["humidity"];
    cityWind = values["windSpeed"];
    cityWindDir = getCompassDirection(values["windDirection"]);
    cityPressure = values["pressureSeaLevel"];
    cityVisibility = values["visibility"];
    cityUV = values["uvIndex"];
    int code = values["weatherCode"];
    cityCondition = getWeatherDescription(code);
    String timeUTC = doc["data"]["time"];     // e.g., "2025-05-26T08:07:00Z"
    int hourUTC = timeUTC.substring(11, 13).toInt(); // Convert UTC time string to hour (simplified for categorization)
    int minuteUTC = timeUTC.substring(14, 16).toInt();
    int hourIST = hourUTC + 5;           // Add 5 hours and 30 minutes to convert to IST
    int minuteIST = minuteUTC + 30;
    if (minuteIST >= 60)                // Adjust overflow
    {
      minuteIST -= 60;
      hourIST += 1;
    }
    if (hourIST >= 24) 
    {
      hourIST -= 24;
    }
    // Time Display for day or night in IST
    if (hourIST >= 4 && hourIST < 6) cityTimeOfDay = "Early Morning";
    else if (hourIST >= 6 && hourIST < 9) cityTimeOfDay = "Morning";
    else if (hourIST >= 9 && hourIST < 12) cityTimeOfDay = "Late Morning";
    else if (hourIST >= 12 && hourIST < 14) cityTimeOfDay = "Early Afternoon";
    else if (hourIST >= 14 && hourIST < 17) cityTimeOfDay = "Afternoon";
    else if (hourIST >= 17 && hourIST < 19) cityTimeOfDay = "Evening";
    else if (hourIST >= 19 && hourIST < 21) cityTimeOfDay = "Late Evening";
    else if (hourIST >= 21 || hourIST < 1) cityTimeOfDay = "Night";
    else if (hourIST >= 1 && hourIST < 4) cityTimeOfDay = "Midnight";
    Serial.println(code);
    return "City Temp: " + String(cityTemp, 1) + "C | Feels: " + String(cityFeelsLike, 1) + "C | " + cityCondition;
  }
  else
  {
    Serial.printf("❌ HTTPS Error: %d\n", httpCode);
    https.end();
    return "Error fetching weather";
  }
}
//===================================================================================================================
// --- Compass Code ---
//===================================================================================================================

String getCompassDirection(float degrees)
{
  const char* directions[] =
  {
    "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
    "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"
  };
  int index = (int)((degrees + 11.25) / 22.5) % 16;
  return String(directions[index]);
}
//===================================================================================================================
// --- Weather Code ---
//===================================================================================================================

String getWeatherDescription(int code)
{
  switch (code)
  {
    case 1000: return "Clear";
    case 1100: return "Mostly Clear";
    case 1101: return "Partly Cloudy";
    case 1102: return "Mostly Cloudy";
    case 1001: return "Cloudy";
    case 2000: return "Fog";
    case 2100: return "Light Fog";
    case 4000: return "Drizzle";
    case 4001: return "Rain";
    case 4200: return "Light Rain";
    case 4201: return "Heavy Rain";
    case 5000: return "Snow";
    case 5001: return "Flurries";
    case 5100: return "Light Snow";
    case 5101: return "Heavy Snow";
    case 6000: return "Freezing Drizzle";
    case 6001: return "Freezing Rain";
    case 6200: return "Light Freezing Rain";
    case 6201: return "Heavy Freezing Rain";
    case 7000: return "Ice Pellets";
    case 7101: return "Heavy Ice Pellets";
    case 7102: return "Light Ice Pellets";
    case 8000: return "Thunderstorm";
    default: return "Unknown";
  }
}
//===================================================================================================================
// --- Web UI ---
//===================================================================================================================

void setupWebUI()
{
  server.on("/", []()
  {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8" />
      <meta name="viewport" content="width=device-width, initial-scale=1" />
      <meta http-equiv="refresh" content="10" />
      <title>Room Monitor</title>
      <style>
        body {
          background: #121212;
          color: #e0e0e0;
          font-family: 'Consolas', 'Courier New', monospace;
          margin: 0;
          padding: 20px;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: flex-start;
          min-height: 100vh;
          user-select: none;
        }
        h1 {
          color: #00ffd1;
          margin-bottom: 20px;
          text-shadow: 0 0 8px #00ffd1;
        }
        .container {
          background: #1f1f1f;
          border-radius: 12px;
          padding: 20px 40px;
          box-shadow: 0 0 15px #00ffd1aa;
          width: 320px;
          max-width: 90vw;
        }
        .item {
          font-size: 1.3em;
          margin: 12px 0;
          padding-bottom: 6px;
          border-bottom: 1px solid #00ffd1aa;
          display: flex;
          justify-content: space-between;
        }
        .label {
          font-weight: 700;
          color: #00ffd1;
        }
        .value {
          font-weight: 400;
          color: #b0fefe;
          text-align: right;
        }
        .weather-section {
          margin-top: 20px;
          border-top: 1px solid #00ffd1aa;
          padding-top: 15px;
        }
        .weather-item {
          font-size: 1.1em;
          margin: 8px 0;
          display: flex;
          justify-content: space-between;
        }
        .weather-label {
          font-weight: bold;
          color: #00ffd1;
        }
        .weather-value {
          color: #b0fefe;
          text-align: right;
        }
        footer {
          margin-top: 40px;
          font-size: 0.75em;
          color: #444;
          text-align: center;
          font-family: Arial, sans-serif;
        }
        .animated-gradient {
          background: linear-gradient(270deg, #00ffd1, #00e0ff, #0099ff, #00ffd1);
          background-size: 800% 800%;
          -webkit-background-clip: text;
          -webkit-text-fill-color: transparent;
          animation: gradientBG 12s ease infinite;
        }
        @keyframes gradientBG {
          0%{background-position:0% 50%}
          50%{background-position:100% 50%}
          100%{background-position:0% 50%}
        }
      </style>
      <script>
        let countdown = 10;
        function updateTimer() {
          if(countdown <= 0) return;
          document.getElementById('refreshTimer').textContent = countdown + "s";
          countdown--;
        }
        setInterval(updateTimer, 1000);
        window.onload = updateTimer;

        function formatAMPM(date) {
          let hours = date.getHours();
          const minutes = date.getMinutes().toString().padStart(2, '0');
          const ampm = hours >= 12 ? 'PM' : 'AM';
          hours = hours % 12;
          hours = hours ? hours : 12;
          return hours + ':' + minutes + ' ' + ampm;
        }

        function updateTimes() {
          const nowUTC = new Date();
          const utcStr = formatAMPM(nowUTC) + " UTC";

          const ist = new Date(nowUTC.getTime() + (5.5 * 60 * 60 * 1000));
          const istStr = formatAMPM(ist) + " IST";

          document.getElementById('utcTime').textContent = utcStr;
          document.getElementById('istTime').textContent = istStr;
        }

        setInterval(updateTimes, 1000);
        window.onload = function() {
          updateTimer();
          updateTimes();
        };
      </script>
    </head>
    <body>
      <h1 class="animated-gradient">Room Monitor</h1>
      <div class="container">
    )rawliteral";

    // Basic Sensor Data
    html += "<div class='item'><span class='label'>Temperature:</span><span class='value'>" + String(temp) + " °C</span></div>";
    html += "<div class='item'><span class='label'>Humidity:</span><span class='value'>" + String(hum) + " %</span></div>";
    html += "<div class='item'><span class='label'>Air Quality:</span><span class='value'>" + String(aoq) + "</span></div>";

    // Full Weather Data
    html += "<div class='weather-section'>";
    html += "<div class='weather-item'><span class='weather-label'>City Temperature:</span><span class='weather-value'>" + String(cityTemp, 1) + " °C</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Feels Like:</span><span class='weather-value'>" + String(cityFeelsLike, 1) + " °C</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Condition:</span><span class='weather-value'>" + cityCondition + "</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Humidity:</span><span class='weather-value'>" + String(cityHumidity, 1) + " %</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Wind Speed:</span><span class='weather-value'>" + String(cityWind, 1) + " m/s</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Wind Direction:</span><span class='weather-value'>" + cityWindDir + "</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Pressure:</span><span class='weather-value'>" + String(cityPressure, 2) + " hPa</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>UV Index:</span><span class='weather-value'>" + String(cityUV, 1) + "</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Visibility:</span><span class='weather-value'>" + String(cityVisibility, 1) + " km</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Time of Day (IST):</span><span class='weather-value'>" + cityTimeOfDay + "</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>IST Time:</span><span class='weather-value' id='istTime'>--:--</span></div>";
    html += "<div class='weather-item'><span class='weather-label'>Standard Time:</span><span class='weather-value' id='utcTime'>--:--</span></div>";
    html += "</div>";

    // Close HTML
    html += R"rawliteral(
      </div>
      <footer>Page refreshes in <span id="refreshTimer">10s</span></footer>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html; charset=UTF-8", html);
  });

  server.begin();
  Serial.println("🌐 Web server started at http://" + WiFi.localIP().toString());
}
//===================================================================================================================
// --- String Converter for Matrix Display ---
//===================================================================================================================

String fixUTF8(String input) //Not used anymore .... I used this for printing Degree Symbol in the Marix then left the IDea.,,
{
  input.replace("°", "\xB0"); // replaces UTF-8 degree with extended ASCII degree
  return input;
}
//===================================================================================================================
// --- The End ---
//===================================================================================================================
