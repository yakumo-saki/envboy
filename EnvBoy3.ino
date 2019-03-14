#include <BME280I2C.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <SSD1306.h>

#include <FS.h>
 
// I2C memo
// LCD = 0x3c  ADT = 0x48

ESP8266WebServer server(80);

// LCD Display
SSD1306 display(0x3c, 5, 4);

// Wi-Fi設定保存ファイル
const char* settings = "/wifi_settings.txt";
const char* configured_file = "/config_ok.txt";

bool isNormal = true;

String product_short = "eb3";
String product = "EnvBoy3.3";
String build_date = "2019/03/14";
String product_long = product + " rev 3";

// setup時は、setup用SSID。 normal時は接続先SSID
String ssid = "";
String password = "";
String mDNS = "";
String verticalFlip = "";

void setup()
{
  Serial.begin(115200);
  Serial.println(""); // 1行目にゴミが出るので改行しないと読めない
  Serial.println("");
  Serial.println("Starting " + product);

  // Init I2C Serial
  Wire.begin(5, 4);
  display.init();

  SPIFFS.begin();
  isNormal = SPIFFS.exists(configured_file);

  if (!isNormal) {
    Serial.println("Entering setup mode.");
    setup_setupmode(); 
  } else {
    Serial.println("Entering normal mode.");
    setup_normal(); 
  }
}
 
void loop() {
  if (!isNormal) {
    loop_setupmode(); 
  } else {
    loop_normal(); 
  }
}
