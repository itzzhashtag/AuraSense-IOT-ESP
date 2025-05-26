/*
  Hello This is me Hashtag .....
  This is my Personal Home Project, I use this project in my room for sensing my Room Enviroment and display it over the IS2 based LCD 16x2 display.
  Then that data is again send to another ESP over Wifi thorught a common Wifi..! 
  This Code also makes sure the Client & Wifi stays connected or retry...!
  Name: Aniket Chowdhury [Hashtag]
  Email: micro.aniket@gmail.com
  GitHub: https://github.com/itzzhashtag
  Instagram: https://instagram.com/itzz_hashtag
  LinkedIn: https://www.linkedin.com/in/itzz-hashtag/
*/

//Change Values and Data before using the code

//===================================================================================================================
// --- Libraries Used ---
//===================================================================================================================
#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_PCF8574.h>
#define DHTPIN 13                    // what pin DHT22 is connected to                <----------------------------------
#define DHTTYPE DHT22
const char* ssid = "Hash~2Ghz";

//===================================================================================================================
// --- Variables and Constants ---
//===================================================================================================================
const char* ssid = "your_ssid";
const char* password = "ssid_password";
IPAddress local_IP("your_local_ip");// 🚀 Correct Static IP for Server ESP32
IPAddress gateway("your_gateway_ip");//anything ending with like 192.***.**.1
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(80);
LiquidCrystal_PCF8574 lcd1(0x27);
LiquidCrystal_PCF8574 lcd2(0x23);
DHT dht(DHTPIN, DHTTYPE);     //// Initialize DHT sensor for normal 16mhz Arduino
int h;                        //Stores humidity value
float t;                      //Stores temperature value
int threshold1 = 80;          //Threshold level for Air Quality
int threshold2 = 150;
const int aqsensor = 33;      //output of mq135 connected to A0 pin of Arduino
int counter = 1, x = 0, y = 0, flag1 = 0, flag2 = 0;
int led_G = 25;                      //red led point                                 <----------------------------------
int led_R = 32;                     //Green led point                                <----------------------------------
int led_B = 34;                     //blue led point                                 <----------------------------------

//===================================================================================================================
// --- Custum Character for LCD ---
//===================================================================================================================
byte Skull[8] = {
  0b00000,
  0b01110,
  0b10101,
  0b11011,
  0b01110,
  0b01110,
  0b00000,
  0b00000
};
byte ok[8] = {
  B00000,
  B00000,
  B00000,
  B00001,
  B00001,
  B00111,
  B00111,
  B00111,
};
byte st1[8] = {
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
};
byte st2[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
};
byte lt1[8] = {
  B00000,
  B00011,
  B00001,
  B11111,
  B11111,
  B00001,
  B00011,
  B00000,
};
byte lt2[] = {
  B00000,
  B11000,
  B10000,
  B11111,
  B11111,
  B10000,
  B11000,
  B00000,
};
byte dg[8] = {
  B00010,
  B00101,
  B00010,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte Heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};
byte Heart2[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};
byte Bell[8] = {
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b00000,
  0b00100,
  0b00000
};
int  sleepyFace[2][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0}
};
int  happyFace[2][16] = {
  {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0}
};
int happyFaceLips[2][16] = {
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}
};
int  normalFace[2][16] = {
  {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0}
};
int  normalFaceLips[2][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}
};
int clk = 90;

//===================================================================================================================
// --- Setup ---
//===================================================================================================================
void setup()
{

  Serial.begin(115200);
  lcd1.begin(16, 2);
  lcd1.setBacklight(255);  // Turn on backlight
  lcd2.begin(16, 2);
  lcd2.setBacklight(255);

  pinMode(led_R, OUTPUT);
  pinMode(led_G, OUTPUT);
  pinMode(led_B, OUTPUT);
  digitalWrite(led_R, HIGH);
  digitalWrite(led_G, HIGH);
  digitalWrite(led_B, HIGH);
  pinMode (aqsensor, INPUT); // MQ135 is connected as INPUT to arduino
  dht.begin();

  lcd2.createChar(1, Skull);
  lcd2.createChar(2, Heart);
  lcd1.createChar(0, Heart2);
  lcd1.createChar(3, dg);
  lcd1.createChar(4, Bell);
  lcd2.createChar(4, Bell);
  lcd2.createChar(5, ok);
  lcd1.createChar(6, st1);
  lcd1.createChar(7, st2);
  lcd1.createChar(8, lt1);
  lcd1.createChar(9, lt2);

  esp_booting();
  booting();
  faces();
  client_connect();

}

//===================================================================================================================
// --- Loop ---
//===================================================================================================================
void loop()
{

  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("Client connected");
    while (client.connected())
    {
      int ppm = analogRead(aqsensor);
      if (clk < 5000)
      {
        clk += 1;
        Serial.print("CLOCK : ");
        Serial.println(clk);
        //read MQ135 analog outputs at A0 and store it in ppm
        Serial.print("Air Quality: ");   //print message in serail monitor
        Serial.println(ppm);             //print value of ppm in serial monitor
        lcd2.setCursor(0, 0);            // set cursor of lcd to 1st row and 1st column
        lcd2.print("Air Quality: ");     // print message on lcd
        lcd2.print(ppm);                 // print value of MQ135
        lcd2.print(" ") ;
        if (ppm > threshold1 && ppm < threshold2)  // check is ppm is greater than threshold or not
        {
          lcd2.setCursor(0, 1);        //jump here if ppm is greater than threshold
          lcd2.print(" AQ Level HIGH ");
          Serial.println("AQ Level HIGH");
          lcd2.write(byte(4));
          lcd2.print("  ");
          lcd2.setCursor(0, 1);
          digitalWrite(led_R, LOW);
          digitalWrite(led_G, LOW);
          digitalWrite(led_B, HIGH);
        }
        else if (ppm > threshold2)
        {
          lcd2.setCursor(1, 1);        //jump here if ppm is greater than threshold
          lcd2.print("AQ Lvl DANGER ");
          lcd2.write(byte(1));
          lcd2.print("  ");
          Serial.println("AQ Level DANGER");
          lcd2.setCursor(0, 1);
          //tone(led_R,1000,200);         //blink led with turn on time 1000mS, turn off time 200mS
          digitalWrite(led_R, LOW);
          digitalWrite(led_G, HIGH);
          digitalWrite(led_B, HIGH);

        }
        else
        {
          //digitalWrite(led,LOW);   //jump here if ppm is not greater than threshold and turn off LED
          lcd2.setCursor(0, 1);
          lcd2.print (" AQ Level Good ");
          lcd2.write(byte(2));
          lcd2.print("  ") ;
          Serial.println("AQ Level Good");
          digitalWrite(led_R, HIGH);
          digitalWrite(led_G, LOW);
          digitalWrite(led_B, HIGH);
        }
        h = (dht.readHumidity()) - 5; //Read data and store it to variables h (humidity) and t (temperature)
        t = (dht.readTemperature()) - 3; // Reading temperature or humidity takes about 250 milliseconds!
        Serial.print("Humidity: ");//Print temp and humidity values to serial monitor
        Serial.print(h);
        Serial.print(" %, Temp: ");
        Serial.print(t);
        Serial.println(" ° Celsius");
        //Print temp and humidity values to LCD Display
        lcd1.setCursor(0, 0);
        lcd1.print(" ");
        lcd1.write(byte(6));
        //lcd1.write(byte(8));
        lcd1.print(" LIVE Stats ");  // <--------------------
        //lcd1.write(byte(9));
        lcd1.write(byte(7));
        lcd1.print(" ");
        lcd1.setCursor(0, 1);
        lcd1.print("T:");
        lcd1.print(t);
        lcd1.write(byte(3));
        lcd1.print("C");
        lcd1.setCursor(11, 1);
        lcd1.print("H:");
        lcd1.print(h);
        lcd1.print("%");
        delay(1000); //Delay 1 sec.
      }
      else
      {
        clk = 0;
        lcd1.clear();
        lcd2.clear();
        esp_booting();
        lcd1.clear();
        lcd2.clear();

        client_connect();
        lcd1.clear();
        lcd2.clear();

      }
      delay(1000);
      int temp = t;  // Direct integer value
      int hum = h;
      int aoq = ppm; // 🔹 Send raw integers using binary transmission
      client.write((uint8_t*)&temp, sizeof(temp));
      client.write((uint8_t*)&hum, sizeof(hum));
      client.write((uint8_t*)&aoq, sizeof(aoq));
      Serial.printf("📤 Sent: T=%d, H=%d, AQ=%d\n", temp, hum, aoq);
      delay(1000);
    }
    client.stop();
  }
}

//===================================================================================================================
// --- Networking and Connect ---
//===================================================================================================================
void client_connect()
{
  if (!server.available())
  {
    if (flag2 == 0)
    {
      flag2 = 1;
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Awaiting Client!");
      lcd1.setCursor(0, 1);
      lcd1.print("       ^^");
      lcd2.setCursor(0, 0);
      lcd2.print("   Connecting  ");
      while (!server.available())
      {
        delay(500);
        if (x < 16)
        {
          lcd2.setCursor(x, 1);
          lcd2.print("=");
          x++;
        }
        else
        {
          x = 0;
          lcd2.setCursor(0, 1);
          lcd2.print("                ");
        }
      }
      Serial.println("⏳ Waiting for client to connect...");
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print(" Client Found.! ");
      lcd2.setCursor(0, 0);
      lcd2.print("   Connected!   ");
      delay(3000);
      lcd1.clear();
      lcd2.clear();
    }
    else if (flag2 == 1)
    {
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("  Client Lost!");
      lcd1.setCursor(0, 1);
      lcd1.print("      T_T");
      lcd2.setCursor(0, 0);
      lcd2.print("  Reconnecting  ");
      x = 0;
      while (!server.available())
      {
        delay(500);
        if (x < 16)
        {
          lcd2.setCursor(x, 1);
          lcd2.print("=");
          x++;
        }
        else
        {
          x = 0;
          lcd2.setCursor(0, 1);
          lcd2.print("                ");
        }
      }
      Serial.println("⏳ Waiting for client to Reconnect...");
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print(" Client Found.! ");
      lcd2.setCursor(0, 0);
      lcd2.print("  Re-Connected  ");
      delay(3000);
      lcd1.clear();
      lcd2.clear();
    }
  }
}

void esp_booting()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (flag1 == 0)
    {
      flag1 = 1;
      Serial.println("🚀 ESP32 Server Starting...");// 🔹 Force Wi-Fi to use static IP
      lcd1.setCursor(4, 0);
      lcd1.print(" ESP-32");
      lcd1.setCursor(0, 1);
      lcd1.print("       ^^");
      delay(500);
      lcd2.setCursor(0, 0);
      lcd2.print("Wifi Connecting!");
      delay(1000);
      WiFi.mode(WIFI_STA);                        // Ensure station mode
      WiFi.config(local_IP, gateway, subnet);     // Apply static IP
      WiFi.begin(ssid, password);                 // 🔹 Now connect to Wi-Fi
      lcd2.setCursor(0, 1);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        if (y < 16)
        {
          lcd2.setCursor(y, 1);
          lcd2.print("=");
          y++;
        }
        else
        {
          y = 0;
          lcd2.setCursor(0, 1);
          lcd2.print("                ");
        }

      }
      Serial.println("\n✅ Connected to Wifi!");
      Serial.print("🔹 Server IP: ");
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("      Wifi ");
      delay(500);
      lcd1.setCursor(0, 1);
      lcd1.print("  Connected..!");
      delay(500);
      lcd2.setCursor(0, 0);
      lcd2.print("   Server IP: ");
      Serial.println(WiFi.localIP());  // This should now be `192.168.**.** `
      lcd2.setCursor(1, 1);
      lcd2.print(WiFi.localIP());
      lcd2.setCursor(9, 1);
      lcd2.print("***.**");
      delay(3000);
      lcd1.clear();
      lcd2.clear();
      server.begin();
      Wire.begin(21, 22);
    }
    else if (flag1 == 1)
    {
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("      Wifi");
      lcd1.setCursor(0, 0);
      lcd1.print("  Disconnected");
      lcd2.setCursor(0, 0);
      delay(500);
      lcd2.print("Trying Reconnect");
      WiFi.mode(WIFI_STA);                        // Ensure station mode
      WiFi.config(local_IP, gateway, subnet);     // Apply static IP
      WiFi.begin(ssid, password);                 // 🔹 Now connect to Wi-Fi
      lcd2.setCursor(0, 1);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        if (y < 16)
        {
          lcd2.setCursor(y, 1);
          lcd2.print("=");
          y++;
        }
        else
        {
          y = 0;
          lcd2.setCursor(0, 1);
          lcd2.print("                ");
        }

      }
      Serial.println("\n✅ ReConnected to Wifi!");
      Serial.print("🔹 Server IP: ");
      lcd1.clear();
      lcd2.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("      Wifi ");
      delay(500);
      lcd1.setCursor(0, 1);
      lcd1.print(" ReConnected..! ");
      delay(500);
      lcd2.setCursor(0, 0);
      lcd2.print("   Server IP:  ");
      Serial.println(WiFi.localIP());  // This should now be `192.168.**.** `
      lcd2.setCursor(1, 1);
      lcd2.print(WiFi.localIP());
      lcd2.setCursor(9, 1);
      lcd2.print("***.**");
      delay(3000);
      lcd1.clear();
      lcd2.clear();
      server.begin();
      Wire.begin(21, 22);
    }
  }
}

//===================================================================================================================
// --- Booting Animation LCD  ---
//===================================================================================================================
void booting()
{
  lcd1.setCursor(3, 0);
  lcd1.print("Booting. ");
  delay(500);
  lcd1.setCursor(3, 0);
  lcd1.print("Booting.. ");
  delay(500);
  lcd1.setCursor(3, 0);
  lcd1.print("Booting... ");
  delay(500);
  lcd1.setCursor(0, 0);
  lcd1.print("-----******-----");
  delay(500);
  lcd1.setCursor(3, 1);
  lcd1.print(" Live  OS ");
  delay(1500);
  lcd2.setCursor(0, 0);
  lcd2.print("-> Booting... <-");
  lcd2.setCursor(1, 1);
  delay(800);
  lcd2.write(byte(2));
  lcd2.print("  Complete  ");
  lcd2.write(byte(2));
  delay(2000);
  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("    Welcome ");
  lcd1.write(byte(4));
  lcd1.print("   ");
  delay(1300);
}

//===================================================================================================================
// --- Face Animation for LCD 16*2  ---
//===================================================================================================================
void faces()
{
  printFace(sleepyFace);
  delay(1500);
  printFace(normalFace);
  delay(400);
  printFace(sleepyFace);
  delay(60);
  printFace(normalFace);
  delay(1000);
  printFace(sleepyFace);
  delay(60);
  printFace(normalFace);
  delay(600);
  printFaceLips(normalFaceLips);
  delay(1000);

  printFace(happyFace);
  printFaceLips(happyFaceLips);
  delay(1000);

  lcd2.setCursor(15, 1);
  lcd2.write(byte(5));
  delay(3000);
}
void printFace(int emo[][16])
{
  lcd1.clear();
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 16; j++) {
      lcd1.setCursor(j, i);
      if (emo[i][j] == 1) {
        lcd1.write(255);
      }
    }
  }
}
void printFaceLips(int emo[][16])
{
  lcd2.clear();
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 16; j++) {
      lcd2.setCursor(j, i);
      if (emo[i][j] == 1) {
        lcd2.write(255);
      }
    }
  }
}
//===================================================================================================================
// --- End ---
//===================================================================================================================
