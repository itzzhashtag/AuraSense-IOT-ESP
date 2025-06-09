/*
  Hello This is me Hashtag .....
  This is my Personal Home Automation Work, I use this project in my room for sensing my Room Enviroment and receive data from other ESP.
  Then that data is displayed LCD.
  This Code also makes sure the ESP2 and Wifi stays connected or retry...!
  Also show appropriate data and Alert display for any Gas leakage..!!
   
  Name: Aniket Chowdhury [Hashtag]
  Email: micro.aniket@example.com
  GitHub: https://github.com/itzzhashtag
  Instagram: https://instagram.com/itzz_hashtag
  LinkedIn: https://www.linkedin.com/in/itzz-hashtag/
*/

//Open File->Preferences->Additional Boards Manager URL's ->(Copy and Paste) "https://dl.espressif.com/dl/package_esp32_index.json,http://arduino.esp8266.com/stable/package_esp8266com_index.json"
//ALso Change Values and Data before using the code (Changes needed) 
// Line Number -> 28,29,31,32,35,36,37  
// Line Number -> 150 156 [Calibrate the values according to your Data Available]

//=====================================================
// --- Libraries Used ---
//=====================================================
#include <WiFi.h>
#include <LiquidCrystal_PCF8574.h>
//=====================================================
// --- Wi-Fi and Server Constants ---
//=====================================================
const char* ssid = "your_ssid";           // Wifi SSID
const char* password = "ssid_password";   // Wifi Passcode

IPAddress local_IP("Your_ESP3_IP");       // Static IP for ESP1
IPAddress gateway("Your_static_Ip");      // Routers IP - Wifi
IPAddress subnet(255, 255, 255, 0);

const char* esp1_ip = "Your_ESP1_IP";     // ESP1 IP
const char* esp2_ip = "Your_ESP2_IP";     // ESP2 IP
const char* esp3_ip = "Your_ESP3_IP";     // ESP3 IP
const int esp1_port = 83;                 // ESP1 Server Port
const int esp2_port = 81;                 // ESP2 Server Port
const int esp3_port = 82;                 // ESP3 Server Port
WiFiServer server(esp3_port);
//=====================================================
// --- Matrix Pins ---
//=====================================================
#define MQ5_PIN 34                        // MQ5 Sensor iNPUT
#define R_Led 15                          // Led Blinker
#define Buzz 18                           // Buzzer Input

//===============================================
//--- Global State Variables ---
//===============================================
LiquidCrystal_PCF8574 lcd(0x23);          // Initialize LCD Matrix to (16x2)
int mq5Value = 0;                         // Stores MQ5 value
int y = 0, f = 0;                         // Flag Variables
int mq5Value = 0;                         // Store MQ5 Values
//=====================================================
// --- Setup ---
//=====================================================
void setup()
{
  Serial.begin(115200);                   // Serial baud set to 115200
  lcd.begin(16, 2);                       // LCD Matrix set to 16x2
  lcd.setBacklight(255);                  // Turn on backlight
  pinMode(R_Led, OUTPUT);                 // R_Led set as INPUT
  pinMode(Buzz, OUTPUT);                  // Buzzer set as INPUT        
  digitalWrite(R_Led, HIGH);
  digitalWrite(Buzz, HIGH);
  wifi_connect();                         // Start The Wifi connection
}
//=====================================================
// --- Loop ---
//=====================================================
void loop()
{
  if (WiFi.status() != WL_CONNECTED)      // Check Wifi each time and retry if not Connected
    wifi_connect();
  mq5Value = analogRead(MQ5_PIN);         // Read Analogue MQ5 Values
  digitalWrite(R_Led, HIGH);
  lcd.setCursor(0, 0);                    // Set cursor of lcd 
  lcd.print(" LPG : ");                   // Appropriate display for LCD
  lcd.print(mq5Value);
  lcd.setCursor(11, 0);
  lcd.print("ppm");
  showstat(mq5Value);                     // Sends Value to showstat function
  Serial.println("MQ135 Raw: " + String(mq5Value));
  WiFiClient client;
  if (client.connect(esp2_ip, esp2_port))
  {
    client.write((uint8_t*)&mq5Value, sizeof(mq5Value));
    Serial.printf("📤 Sent: %d to ESP-2(Server)\n", mq5Value);
    client.stop();
  }
  else
  {
    Serial.println("Connection to ESP2 failed");
  }
  delay(1500);
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
//=====================================================
// --- Display ---
//=====================================================
void showstat(int val)                    // Appropriate display for LCD
{
  lcd.setCursor(0, 1);                    // Set cursor of lcd 
  if (val > 700)
  {
    lcd.print("Gas Leaking..!! ");
    digitalWrite(R_Led, HIGH);
    warningBuzz(3, 250);
  }
  else if (val > 450)
  {
    lcd.print("Status: Caution! ");
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
    lcd.print("Status:  SAFE    ");
    digitalWrite(R_Led, LOW);
    f = 0;
    delay(500);
  }
}
//=====================================================
// --- Buzzer Warning ---
//=====================================================
void warningBuzz(int times, int duration )  // Buzzer
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(Buzz, LOW);              // Turn buzzer ON
    delay(duration);                      // Wait duration seconds
    digitalWrite(Buzz, HIGH);             // Turn buzzer OFF
    delay(duration);
  }
}
//=====================================================
// --- The End ---
//=====================================================
