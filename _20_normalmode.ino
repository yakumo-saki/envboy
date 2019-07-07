#include <Tsl2561.h>

extern BME280I2C bme;
extern Tsl2561 Tsl;

int counter;


// 最後に取得した温度
extern float lastTemp;
extern float lastHumidity;
extern float lastPressure;
extern float lastLuxFull;
extern float lastLuxIr;

void setup_normal() {
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);

  showStartupScreen(product_long);

  initializeWiFi();

  showWifiInfo(WiFi.localIP().toString(), mDNS);

  bme_setup();
  tsl_setup();

  server_setup();

  delay(500);

  // setupモードに入りやすくするための処理
  String filename = configured_file;
  filename = filename + "tmp";
  SPIFFS.rename(configured_file, filename);
  showWaitForReconfig();
  SPIFFS.rename(filename, configured_file);

  display.clear();
}

void loop_normal() {

  // WiFiが繋がってなければ意味がないので接続チェック
  checkAndReconnectWifi();
  
  server.handleClient();

  char buf[16];
  
  show_main(WiFi.localIP().toString(), mDNS);
  
  readData();

  delay(2000);
}

void initializeWiFi() {
  // 設定を読んで
  File f = SPIFFS.open(settings, "r");
  ssid = f.readStringUntil('\n');
  password = f.readStringUntil('\n');
  mDNS = f.readStringUntil('\n');  
  f.close();

  ssid.trim();
  password.trim();
  mDNS.trim();

  Serial.println("SSID: " + ssid);
  Serial.println("PASS: " + password);
  Serial.println("mDNS: " + mDNS);

  WiFi.begin(ssid.c_str(), password.c_str());

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCount++;
    if (retryCount % 10 == 0) {
      Serial.println("");      
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // mDNS responder
  if ( MDNS.begin ( mDNS.c_str() ) ) {
    Serial.println ( "mDNS responder started " + mDNS );
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println ( "mDNS responder failed " + mDNS );
    mDNS = "no mDNS";
  }
}


void readData() {

  if (true) {
    readDataBme280();
  } else {
    lastTemp = 0;
    lastHumidity = 0;
    lastPressure = 0;    
  }

  if (Tsl.available()) {
    readDataTsl2561();
  } else {
    lastLuxFull = 0;
    lastLuxIr = 0;
  }
  
}

void checkAndReconnectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("WiFi is down. restart");
  WiFi.disconnect();

  int retryCount = 0;
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    retryCount++;
    if (retryCount % 15 == 0) {
      retryCount = 0;
      WiFi.disconnect();   
      WiFi.begin(ssid.c_str(), password.c_str());
      Serial.println("");
      Serial.print("Still reconnecting WiFi");
    }
  }

  Serial.println("");
  Serial.println("WiFi reconnected.");

}
