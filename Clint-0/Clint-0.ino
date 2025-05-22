/*
  Hello This is me Hashtag .....
  This is my Personal Home Automation Work, I use this project in my room for sensing my Room Enviroment and receive data from other ESP.
  Then that data is displayed to Dot Matrix with few basic Animations.
  This Code also makes sure the Client & Wifi stays connected or retry...!
  This Also has the Web based UI to show data over Web.. The Ip will be given to you..!
  Name: Aniket Chowdhury [Hashtag]
  Email: micro.aniket@example.com
  GitHub: https://github.com/itzzhashtag
  Instagram: https://instagram.com/itzz_hashtag
  LinkedIn: https://www.linkedin.com/in/itzz-hashtag/
*/

//Change Values and Data before using the code
//(Changes needed) Line Number ->32,33,34,36,37,299,300

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
const unsigned long weatherInterval = 3 * 60 * 1000; ////minutes(the main changer) * Seconds * milliseconds to update weather report
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
      //weather_rp = fixUTF8(weather_rp); // ensure ASCII-friendly display
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
    char buffer[80];
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
String weather_report()
{
  const char* city = "Your_City";
  const char* apiKey = "your_api_key"; //G
  String url = "https://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(apiKey) + "&units=metric";
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
    int tempIndex = payload.indexOf("\"temp\":");
    int condIndex = payload.indexOf("\"description\":\"");
    float temp = 0;
    String condition = "Unknown";

    if (tempIndex != -1)
    {
      int commaIndex = payload.indexOf(",", tempIndex);
      temp = payload.substring(tempIndex + 7, commaIndex).toFloat();
    }

    if (condIndex != -1)
    {
      int start = condIndex + 15;
      int end = payload.indexOf("\"", start);
      condition = payload.substring(start, end);
    }
    return "City Temp: " + String(temp, 1) + "C | Weather: " + condition;
  }
  else
  {
    Serial.printf("❌ HTTPS Error: %d\n", httpCode);
    return "Error fetching weather";
  }
  https.end();
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
        }
        /* Updated weather styles */
        .weather-section {
          margin-top: 20px;
          border-top: 1px solid #00ffd1aa; /* Separator line */
          padding-top: 15px;
          text-align: left; /* Align weather info to the left */
        }
        .weather-item {
          font-size: 1.1em; /* Slightly smaller than main items */
          margin: 8px 0;   /* Spacing between weather lines */
          color: #b0fefe; /* Default text color for weather values */
        }
        .weather-label {
          font-weight: bold;
          color: #00ffd1;   /* Accent color for labels like "City Temp:", "Condition:" */
          margin-right: 8px; /* Space after the label */
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
      </script>
    </head>
    <body>
      <h1 class="animated-gradient">Room Monitor</h1>
      <div class="container">
    )rawliteral";
    // Append dynamic data
    html += "<div class='item'><span class='label'>Temperature:</span><span class='value'>" + String(temp) + " °C</span></div>";
    html += "<div class='item'><span class='label'>Humidity:</span><span class='value'>" + String(hum) + " %</span></div>";
    html += "<div class='item'><span class='label'>Air Quality:</span><span class='value'>" + String(aoq) + "</span></div>";
    // --- MODIFIED WEATHER DISPLAY ---
    String local_weather_rp = weather_rp; // Use a local copy
    if (local_weather_rp.startsWith("Error") || local_weather_rp.startsWith("Loading"))
    {
      // Display error or loading message directly but styled
      html += "<div class='weather-section'>";
      html += "  <div class='weather-item'><span class='weather-label'>Weather:</span>" + local_weather_rp + "</div>";
      html += "</div>";
    }
    else
    {
      int pipePosition = local_weather_rp.indexOf('|');
      String cityTempInfo = "";
      String weatherConditionInfo = "";
      if (pipePosition != -1) {
        cityTempInfo = local_weather_rp.substring(0, pipePosition);
        weatherConditionInfo = local_weather_rp.substring(pipePosition + 1);
        // Clean up the extracted parts
        cityTempInfo.replace("City Temp:", "");
        weatherConditionInfo.replace("Weather:", "");
        cityTempInfo.trim();
        weatherConditionInfo.trim();
        // --- Ensure cityTempInfo value ends with °C ---
        String tempNumberPart = cityTempInfo; // e.g., "32.1°C", "32.1 C", "32.1C"
        // Remove existing unit suffixes to isolate the number
        if (tempNumberPart.endsWith("°C")) {
          tempNumberPart.remove(tempNumberPart.length() - 2); // Remove "°C"
        } else if (tempNumberPart.endsWith(" C")) { // Handles " C" (with a space)
          tempNumberPart.remove(tempNumberPart.length() - 2); // Remove " C"
        } else if (tempNumberPart.endsWith("C")) { // Handles "C" (without a space)
          tempNumberPart.remove(tempNumberPart.length() - 1); // Remove "C"
        }
        tempNumberPart.trim(); // Ensure no trailing/leading spaces on the number part
        // Reconstruct cityTempInfo with the number and the °C unit
        cityTempInfo = tempNumberPart + "°C";
        // --- End of °C enforcement ---
        html += "<div class='weather-section'>"; // Container for the improved weather info
        html += "  <div class='weather-item'><span class='weather-label'>City Temp:</span>" + cityTempInfo + "</div>";
        html += "  <div class='weather-item'><span class='weather-label'>Condition:</span>" + weatherConditionInfo + "</div>";
        html += "</div>";
      } else {
        // Fallback if parsing fails for an unexpected format
        html += "<div class='weather-section'>";
        html += "  <div class='weather-item'><span class='weather-label'>Weather Info:</span>" + local_weather_rp + "</div>";
        html += "</div>";
      }
    }
    // --- END OF MODIFIED WEATHER DISPLAY ---
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
