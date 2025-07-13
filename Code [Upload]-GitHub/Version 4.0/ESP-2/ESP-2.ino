//=====================================================
// --- Libraries Used ---
//=====================================================
#include <WiFi.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <DHT.h>
//=====================================================
// --- Network Details and Constants ---
//=====================================================
const char* ssid = "#@5H-2G~M0d^l-4o4";
const char* password = "Ac~$12@2^^2@@2$~aC";

IPAddress local_IP(192, 168, 0, 185);    // Static IP for ESP2
IPAddress gateway(192, 168, 0, 1);       // Routers IP - Wifi
IPAddress subnet(255, 255, 255, 0);

const char* esp1_ip = "192.168.0.186";   // ESP1 IP
const char* esp2_ip = "192.168.0.185";   // ESP2 IP
const char* esp3_ip = "192.168.0.184";   // ESP3 IP
const int esp1_port = 83;                 // ESP1 Server Port
const int esp2_port = 81;                 // ESP2 Server Port
const int esp3_port = 82;                 // ESP3 Server Port

WiFiServer server(esp2_port);

//=====================================================
// --- Matrix Pins ---
//=====================================================
#define DHTPIN 32                         // what pin DHT22 is connected to  
#define DHTTYPE DHT22
const int MQ7sensor = 35;                 // MQ7 Sensor Input
const int MQ135sensor = 34;               // MQ135 Sensor Input
const int led_R = 5;                      // Green Led Input
const int led_G = 18;                     // Red Led Input
const int led_Y = 19;                     // Yellow Led Input
const int Buzz = 23;                      // Buzzer Input

//===============================================
//--- Global State Variables ---
//===============================================
hd44780_I2Cexp lcd;                       // Initialize LCD Matrix to (20x4)
DHT dht(DHTPIN, DHTTYPE);                 // Initialize DHT sensor
int h = 0, t1 = 0;                        // Stores Humidity and Temperature Values[int]
float t = 0;                              // Stores Temperature value
int ppm = 0;                              // Stores AoQ value
int mq7Val = 0;                           // Stores MQ7 value
int mq5Val = 10;                          // Stores MQ5 value
int mq135Val = 0;                         // Stores MQ135 value
int counter = 1, x = 0, y = 0;            // Counters
int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;   // Flag VariablesLeakage
int th1 = 0;                              // Threshold for Excellent AQ
int th2 = 200;                            // Threshold for Good AQ
int th3 = 400;                            // Threshold for Fair AQ
int th4 = 580;                            // Threshold for Moderate AQ
int th5 = 850;                            // Threshold for Poor AQ
int th6 = 1100;                            // Threshold for Hazardous AQ
int mq5Alrm = 0;
//======================================================
// --- Custum Character for LCD ---
///=====================================================
byte Skull[8] = {B00000, B01110, B10101, B10101, B11111, B01010, B01110, B00000,};
byte Heart[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000};
byte Bell[8] = {0b00100, 0b01110, 0b01110, 0b01110, 0b11111, 0b00000, 0b00100, 0b00000};
byte st1[8] = {B00011, B00011, B00011, B00011, B00011, B00011, B00011, B00011,};
byte st2[8] = {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000,};
//=====================================================
// --- Setup ---
//=====================================================
void setup()
{
  Serial.begin(115200);                    // Serial baud set to 115200
  lcd.begin(20, 4);                        // LCD Matrix set to 20x4
  lcd.setBacklight(255);                   // Turn on backlight
  pinMode(led_R, OUTPUT);                  // led_R set as OUTPUT
  pinMode(led_G, OUTPUT);                  // led_G set as OUTPUT
  pinMode(led_Y, OUTPUT);                  // led_Y set as OUTPUT
  pinMode(Buzz, OUTPUT);                   // Buzzer set as OUTPUT
  digitalWrite(led_R, HIGH);               // Turns Red Led ON
  digitalWrite(led_G, HIGH);               // Turns Green Led ON
  digitalWrite(led_Y, HIGH);               // Turns Yellow Led ON
  digitalWrite(Buzz, HIGH);                // Set Buzzer to OFF State
  pinMode (MQ135sensor, INPUT);            // MQ135 set as INPUT
  pinMode (MQ7sensor, INPUT);              // MQ7 set as INPUT
  dht.begin();
  lcd.createChar(0, Skull);
  lcd.createChar(2, Heart);
  lcd.createChar(4, Bell);
  lcd.createChar(6, st1);
  lcd.createChar(7, st2);
  Wire.begin(21, 22);
  esp_booting();                          // Start The Wifi connection
}
//=====================================================
// --- Loop ---
//=====================================================
void loop()
{
  if (WiFi.status() != WL_CONNECTED)      // Check Wifi each time and retry if not Connected
    esp_booting();
  Sub_Client();                           // Receives Data from ESP3
  if (mq5Alrm == 0)                       // If LPG Normal State
  {
    mq135Val = analogRead(MQ135sensor);   // Read Analogue MQ135 Values
    mq7Val = analogRead(MQ7sensor);       // Read Analogue MQ7 Values
    ppm = (mq135Val + mq7Val) / 2;        // Calculate ppm Values
    h = (dht.readHumidity()) - 5;         // Calibrate h according to the users Sensor
    t = (dht.readTemperature()) - 3;      // Calibrate t according to the users Sensor
    if (t < -40 || t > 100)
      t = 0;
    if (h < -3 || h > 100)
      h = 0;
    //if (mq135Val < 0 || mq135Val < 5000)
    // mq135Val = 0;
    //if (mq7Val < 0 || mq7Val < 5000)
    //  mq7Val = 0;
    Serial.printf("🌀 AoQ: %d | 💧 H: %d%% | 🌡 T: %.1f°C | 🛢 MQ7: %d | 🛢 MQ135: %d | 🛢 MQ5: %d |  Alarm: %s \n", ppm, h, t, mq7Val, mq135Val, mq5Val, mq5Alrm ? "true" : "false");
    lcd.setCursor(2, 0);                  // Set cursor of lcd
    lcd.print(" ");
    lcd.write(byte(6));
    lcd.print(" LIVE Stats ");            // Appropriate display for LCD
    //lcd.write(byte(9));
    lcd.write(byte(7));
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("Co2 :");
    lcd.print(mq135Val);
    lcd.setCursor(12, 1);
    lcd.print("Co :");
    lcd.print(mq7Val);
    lcd.setCursor(0, 2);
    lcd.print("LPG :");
    lcd.print(mq5Val);
    lcd.setCursor(11, 2);
    lcd.print("AoQ :");
    lcd.print(ppm);
    threshold_display();                       // Display AoQ on LCD with ref to ppm
    delay(400);
    Main_Client();                             // Send those Data to ESP1
    lcd.clear();
  }
  else
  {
    digitalWrite(led_R, HIGH);
    digitalWrite(led_G, LOW);
    digitalWrite(led_Y, HIGH);
    lcd.setCursor(4, 0);
    lcd.write(byte(6));
    lcd.print(" WARNING ");
    lcd.write(byte(7));
    lcd.setCursor(0, 1);
    lcd.write(byte(4));
    lcd.print(" LPG Gas : ");
    lcd.print(mq5Val);
    lcd.setCursor(17, 1);
    lcd.print("ppm");
    lcd.setCursor(0, 3);
    lcd.write(byte(0));
    lcd.print("  GAS Leaking..!! ");
    delay(50);
    warningBuzz(3, 300);                       // Buzzer beeps for 3 time with 300 delay
    Main_Client();                             // Sends Data to ESP1
    delay(500);
  }
}
//=====================================================
// --- Aoq Display ---
//=====================================================
void threshold_display()
{
  if (ppm > th1 && ppm <= th2)
  {
    flag2 = 0;
    flag3 = 0;
    lcd.setCursor(0, 3);
    lcd.print(" AoQ Lvl: Excellent");
    lcd.write(byte(2));
    Serial.println("AQ Level Excellent");
    digitalWrite(led_R, LOW);
    digitalWrite(led_G, HIGH);
    digitalWrite(led_Y, LOW);
  }
  else if (ppm > th2 && ppm <= th3)
  {
    flag2 = 0;
    flag3 = 0;
    lcd.setCursor(0, 3);
    lcd.print ("  AoQ Level Good ");
    lcd.write(byte(2));
    lcd.print("  ");
    Serial.println("AQ Level Good");
    digitalWrite(led_R, LOW);
    digitalWrite(led_G, HIGH);
    digitalWrite(led_Y, LOW);
  }
  else if (ppm > th3 && ppm <= th4)
  {
    flag2 = 0;
    flag3 = 0;
    lcd.setCursor(0, 3);
    lcd.print(" AoQ Level: Fair ");
    lcd.write(byte(2));
    lcd.print("  ");
    Serial.println("AQ Level Fair");
    digitalWrite(led_R, LOW);
    digitalWrite(led_G, HIGH);
    digitalWrite(led_Y, HIGH);
  }
  else if (ppm > th4 && ppm <= th5)
  {
    flag2 = 0;
    flag3 = 0;
    lcd.setCursor(0, 3);
    lcd.print(" AoQ Lvl: Moderate ");
    lcd.write(byte(4));
    lcd.print(" ");
    Serial.println("AQ Level Moderate");
    digitalWrite(led_R, LOW);
    digitalWrite(led_G, LOW);
    digitalWrite(led_Y, HIGH);
  }
  else if (ppm > th5 && ppm <= th6)
  {
    flag2 = 0;
    lcd.setCursor(0, 3);
    lcd.print("  AoQ Level Poor ");
    Serial.println("AoQ Level Poor");
    lcd.write(byte(4));
    lcd.print("  ");
    lcd.setCursor(0, 3);
    digitalWrite(led_R, HIGH);
    digitalWrite(led_G, LOW);
    digitalWrite(led_Y, HIGH);
    if (flag3 == 0)
    {
      flag3 = 1;
      warningBuzz(2, 250);
    }
  }
  else if (ppm > th6)
  {
    flag3 = 0;
    lcd.setCursor(0, 3);
    lcd.print(" AoQ Lvl Hazardous ");
    lcd.write(byte(0));
    Serial.println("AQ Level DANGER");
    lcd.setCursor(0, 1);
    digitalWrite(led_R, HIGH);
    digitalWrite(led_G, LOW);
    digitalWrite(led_Y, LOW);
    if (flag2 == 0)
    {
      flag2 = 1;
      warningBuzz(3, 300);
    }
  }
  else
  {
    lcd.setCursor(0, 3);
    lcd.print("     AoQ ERROR ");
    lcd.write(byte(4));
    lcd.print("    ");
  }
}
//=====================================================
// --- Networking and Connect ---
//=====================================================
void esp_booting()
{
  if (flag1 == 0)
  {
    flag1 = 1;
    Serial.println("🚀 ESP32 Server Starting...");
    lcd.setCursor(0, 0);
    lcd.print("       ESP-32");
    lcd.setCursor(0, 1);
    lcd.print("         ^^");
    delay(500);
    lcd.setCursor(0, 2);
    lcd.print("  Wifi Connecting!");
    delay(1000);
    WiFi.mode(WIFI_STA);                        // Ensure station mode
    WiFi.config(local_IP, gateway, subnet);     // Apply static IP
    WiFi.begin(ssid, password);                 // 🔹 Now connect to Wi-Fi
    lcd.setCursor(0, 3);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      if (y < 20)
      {
        lcd.setCursor(y, 3);
        lcd.print("=");
        y++;
      }
      else
      {
        y = 0;
        lcd.setCursor(0, 3);
        lcd.print("                    ");
      }

    }
    Serial.println("\n✅ Connected to Wifi!");
    Serial.print("🔹 Server IP: ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Wifi Connected.!");
    delay(500);
    lcd.setCursor(0, 2);
    lcd.print("     Server IP:");
    Serial.println(WiFi.localIP());  // This should now be `192.168.**.** `
    lcd.setCursor(3, 3);
    lcd.print(WiFi.localIP());
    lcd.setCursor(11, 3);
    lcd.print("***.**");
    delay(4000);
    lcd.clear();
    server.begin();
  }
  else if (flag1 == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Wifi-Disconnected!");
    lcd.setCursor(0, 1);
    delay(500);
    lcd.print("   Reconnecting..");
    WiFi.mode(WIFI_STA);                        // Ensure station mode
    WiFi.config(local_IP, gateway, subnet);     // Apply static IP
    WiFi.begin(ssid, password);                 // 🔹 Now connect to Wi-Fi
    lcd.setCursor(0, 3);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      if (y < 20)
      {
        lcd.setCursor(y, 3);
        lcd.print("=");
        y++;
      }
      else
      {
        y = 0;
        lcd.setCursor(0, 3);
        lcd.print("                    ");
      }
    }
    Serial.println("\n✅ ReConnected to Wifi!");
    Serial.print("🔹 Server IP: ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Wifi Re-Connected.!");
    delay(500);
    lcd.setCursor(0, 2);
    lcd.print("     Server IP:");
    Serial.println(WiFi.localIP());  // This should now be `192.168.**.** `
    lcd.setCursor(3, 3);
    lcd.print(WiFi.localIP());
    lcd.setCursor(11, 3);
    lcd.print("***.**");
    delay(4000);
    lcd.clear();
    server.begin();
  }
}
//=====================================================
// --- CLients [ ESP1 and ESP3 ] ---
//=====================================================
void Sub_Client()
{
  WiFiClient client = server.available();// Receive Data from ESP-3
  if (client)
  {
    Serial.println("ESP-3 got connected to ESP-2..!");
    unsigned long start = millis();  // ✅ Declare the variable
    while ((client.available() < sizeof(mq5Val) + sizeof(mq5Alrm)) && (millis() - start < 1000))
    {
      delay(10); // wait a bit for data to arrive
    }
    if (client.available() >= sizeof(mq5Val) + sizeof(mq5Alrm))
    {
      client.read((uint8_t*)&mq5Val, sizeof(mq5Val));
      client.read((uint8_t*)&mq5Alrm, sizeof(mq5Alrm));
      Serial.printf("📥 Received: %d | Alarm: %s\n", mq5Val, mq5Alrm ? "true" : "false");
    }
    else
    {
      Serial.println("⚠️ Incomplete data received from ESP-3.");
    }
    client.stop();
  }
}
void Main_Client()    //Send Data to ESP-1 to Display those Data's
{
  WiFiClient client2;
  if (client2.connect(esp1_ip, esp1_port))
  {
    t1 = static_cast<int>(t);                             //convert Temp to int
    client2.write((uint8_t*)&t1, sizeof(t1));
    client2.write((uint8_t*)&h, sizeof(h));
    client2.write((uint8_t*)&ppm, sizeof(ppm));
    client2.write((uint8_t*)&mq5Alrm, sizeof(mq5Alrm));
    Serial.printf("📤 Sent: T=%d, H=%d, AoQ=%d, Alarm=%s\n", t1, h, ppm, mq5Alrm ? "true" : "false");
    client2.stop();
    delay(900);
  }
}
//=====================================================
// --- Warning Buzz ---
//=====================================================
void warningBuzz(int times, int duration )
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(Buzz, LOW);        // Turn buzzer ON
    digitalWrite(led_R, HIGH);
    delay(duration);
    digitalWrite(Buzz, HIGH);       // Turn buzzer OFF
    digitalWrite(led_R, LOW);
    delay(duration);
  }
}
//=====================================================
// --- The End ---
//=====================================================
