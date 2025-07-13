//=====================================================
// --- Libraries Used ---
//=====================================================
#include <WiFi.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT.h>
//=====================================================
// --- Wi-Fi and Server Constants ---
//=====================================================
const char* ssid = "#@5H-2G~M0d^l-4o4";
const char* password = "Ac~$12@2^^2@@2$~aC";
IPAddress local_IP(192, 168, 0, 184);    // Static IP for ESP3
IPAddress gateway(192, 168, 0, 1);       // Routers IP - Wifi
IPAddress subnet(255, 255, 255, 0);

const char* esp1_ip = "192.168.0.186";   // ESP1 IP
const char* esp2_ip = "192.168.0.185";   // ESP2 IP
const char* esp3_ip = "192.168.0.184";   // ESP3 IP
const int esp1_port = 83;                 // ESP1 Server Port
const int esp2_port = 81;                 // ESP2 Server Port
const int esp3_port = 82;                 // ESP3 Server Port
WiFiServer server(esp3_port);
//=====================================================
// --- Matrix Pins ---
//=====================================================
#define MQ5_PIN 34                 //Mq5 Sensor
#define R_Led 15                   //Led Blinker
#define Buzz 18
#define DHTPIN 27        // Or any free GPIO pin
#define DHTTYPE DHT11    // Use DHT11 instead of DHT22
int mq5Value = 69;
int mq5Alrm = 0;
int y = 0, f = 0, temp = 0, hum = 0;
LiquidCrystal_PCF8574 lcd(0x23);
DHT dht(DHTPIN, DHTTYPE);
//=====================================================
// --- Setup ---
//=====================================================
void setup()
{
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);  // Turn on backlight
  pinMode(R_Led, OUTPUT);
  pinMode(Buzz, OUTPUT);
  int mq5Value = analogRead(MQ5_PIN);
  digitalWrite(R_Led, HIGH);
  digitalWrite(Buzz, HIGH);
  wifi_connect();
  lcd.setCursor(0, 0);
  lcd.print("Sensor Warming..");
  unsigned long totalSeconds = 5 * 60;  // 5 minutes in seconds
  for (unsigned long i = totalSeconds; i > 0; i--) {
    lcd.setCursor(2, 1);
    unsigned long minutes = i / 60;
    unsigned long seconds = i % 60;
    lcd.print("Wait: ");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);
    lcd.print("    ");  // Clear leftover characters
    mq5Value = analogRead(MQ5_PIN);
    Serial.println("MQ5 Raw: " + String(mq5Value));
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Sensor Ready!");
  delay(2000);  // Optional short message before starting actual loop
  lcd.clear();
}
//=====================================================
// --- Loop ---
//=====================================================
void loop()
{
  if (WiFi.status() != WL_CONNECTED)
    wifi_connect();
  mq5Value = analogRead(MQ5_PIN);
  digitalWrite(R_Led, HIGH);
  lcd.setCursor(0, 0);
  lcd.print(" LPG : ");
  lcd.print(mq5Value);
  lcd.setCursor(11, 0);
  lcd.print("ppm");
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temp);
  lcd.setCursor(4, 1);
  lcd.print(" H:");
  lcd.print(hum);
  showstat(mq5Value);
  Serial.printf("MQ5 Raw: %d | Alarm: %s\n", mq5Value, mq5Alrm ? "true" : "false");
  SendData();
  delay(2000);
  lcd.clear();
}
//=====================================================
// --- Networking Functions ---
//=====================================================
void wifi_connect()
{

  WiFi.mode(WIFI_STA);                        // Ensure station mode
  WiFi.config(local_IP, gateway, subnet);     // Apply static IP
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Wifi-Connecting!");
  delay(1000);
  lcd.setCursor(0, 1);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (y < 16)
    {
      lcd.setCursor(y, 1);
      lcd.print("=");
      y++;
    }
    else
    {
      y = 0;
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
  Serial.println("\n✅ Connected to Wifi!");
  Serial.println("\nConnected to Wifi as ESP3: " + WiFi.localIP().toString());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Server IP: ");
  Serial.print("🔹 Server IP: ");
  Serial.println(WiFi.localIP());  // This should now be `192.168.**.** `
  lcd.setCursor(1, 1);
  lcd.print(WiFi.localIP());
  lcd.setCursor(9, 1);
  lcd.print("***.**");
  delay(3000);
  lcd.clear();
}

void SendData()
{
  WiFiClient client;
  if (client.connect(esp2_ip, esp2_port))
  {
    client.write((uint8_t*)&mq5Value, sizeof(mq5Value));
    client.write((uint8_t*)&mq5Alrm, sizeof(mq5Alrm));
    Serial.printf("📤 Sent: %d | Alarm: %s to ESP-2(Server)\n", mq5Value, mq5Alrm ? "true" : "false");
    Serial.println();
    client.stop();
  }
  else
  {
    Serial.println("Connection to ESP2 failed");
  }
}
//=====================================================
// --- Display ---
//=====================================================
void showstat(int val)
{
  enviro();  // Reads temp and humidity into 'temp' and 'hum'
  
  int safeThreshold, cautionThreshold;
  calculateThresholds(temp, hum, safeThreshold, cautionThreshold);
  if (val > cautionThreshold)
  {
    mq5Alrm = 1;
    lcd.setCursor(0, 1);
    lcd.print(" Gas Leaking..!");
    digitalWrite(R_Led, HIGH);
    warningBuzz(3, 250);
  }
  else if (val > safeThreshold)
  {
    mq5Alrm = 0;
    lcd.setCursor(9, 1);
    lcd.print(" ALERT");
    digitalWrite(R_Led, HIGH);
    if (f == 0)
    {
      f = 1;
      warningBuzz(2, 150);
    }
    else
    {
      delay(200);
    }
  }
  else
  {
    mq5Alrm = 0;
    lcd.setCursor(9, 1);
    lcd.print("  SAFE  ");
    digitalWrite(R_Led, LOW);
    f = 0;
    delay(500);
  }
  // Optional: print thresholds for debug
  Serial.printf("🌡 %d°C | 💧 %d%% | ✅ Safe=%d | ⚠️ Alert=%d | MQ5=%d\n", temp, hum, safeThreshold, cautionThreshold, val);
}
void calculateThresholds(int temp, int hum, int &safeThreshold, int &cautionThreshold)
{
  // Base and max environment
  const int baseTemp = 23, maxTemp = 33;
  const int baseHum  = 47, maxHum  = 98;

  // Target thresholds
  const int safeBase = 1300, safeMax = 2100;
  const int alertBase = 1000, alertMax = 1900;

  // Clamp temp and humidity to expected bounds
  temp = constrain(temp, baseTemp, maxTemp);
  hum  = constrain(hum, baseHum, maxHum);

  // Calculate ratios
  float tempRatio = float(temp - baseTemp) / (maxTemp - baseTemp);  // 0 to 1
  float humRatio  = float(hum - baseHum) / (maxHum - baseHum);      // 0 to 1

  // Give 60% weight to temperature, 40% to humidity (adjustable)
  float combinedRatio = (tempRatio * 0.6) + (humRatio * 0.4);

  // Interpolate thresholds
  safeThreshold = safeBase + (int)((safeMax - safeBase) * combinedRatio);
  cautionThreshold = alertBase + (int)((alertMax - alertBase) * combinedRatio);

  // Ensure minimum gap of 150
  if (safeThreshold - cautionThreshold < 150)
    cautionThreshold = safeThreshold - 150;
}

void enviro()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(hum) || isnan(temp))
  {
    Serial.println("❌ Failed to read from DHT11 sensor!");
  } else {
    Serial.printf("🌡 Temp: %d°C | 💧 Humidity: %d%%\n", temp, hum);
  }
}
//=====================================================
// --- Buzzer Warning ---
//=====================================================
void warningBuzz(int times, int duration )
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(Buzz, LOW);  // Turn buzzer ON
    delay(duration);                     // Wait 0.5 seconds
    digitalWrite(Buzz, HIGH);   // Turn buzzer OFF
    delay(duration);
  }
}
//=====================================================
// --- The End ---
//=====================================================w
