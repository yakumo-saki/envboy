
int counter;

BME280I2C bme;   // Default : forced mode, standby time = 1000 ms
                 // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
                 // Address 0x76

// 最後に取得した温度
float lastTemp;
float lastHumidity;
float lastPressure;

// 設定から読み込む値。
bool doVerticalFlip;

void readConfig() {
  // 設定を読んで
  File f = SPIFFS.open(settings, "r");
  ssid = f.readStringUntil('\n');
  password = f.readStringUntil('\n');
  mDNS = f.readStringUntil('\n');
  verticalFlip = f.readStringUntil('\n');   
  f.close();

  // 読み込んだ値に余計なものがついているので削除
  verticalFlip.trim();
  ssid.trim();
  password.trim();
  mDNS.trim();
  doVerticalFlip = verticalFlip.equals("1");

  Serial.println("SSID: " + ssid);
  Serial.println("PASS: " + password);
  Serial.println("mDNS: " + mDNS);
  Serial.print("flip: ");
  Serial.println((doVerticalFlip ? "TRUE" : "FALSE"));
}

void setupMdns() {
  // mDNS responder
  if ( MDNS.begin ( mDNS.c_str() ) ) {
    Serial.println ( "mDNS responder started " + mDNS );
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println ( "mDNS responder failed " + mDNS );
    mDNS = "no mDNS";
  }  
}

void initializeWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.println("WiFi connection start.");
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCount++;
    if (retryCount % 10 == 0) {
      Serial.println("");      
    }
    if (retryCount > 30) {
      Serial.println("WiFi connection fail. fallback to setup mode");      
      SPIFFS.remove(configured_file);
      Serial.println("done");
      showWiFiErrorScreen();

      while (true) {
        Serial.println("system has halted. please restart.");
        delay(5000);    
      }
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_normal() {
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);

  showStartupScreen();

  readConfig();

  initializeWiFi();
  showWifiInfo();

  setupMdns();

  // init BME
  while(!bme.begin()){
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }
  delay(500);

  // init LPS22HB if found (0x5d)
  if (lps22hb.init()) {
      Serial.println("Barometric sensor initialization succeeded!");
      lps22hb.enableSensor(Sodaq_LPS22HB::OdrOneShot);
      useLPS22HB = true;
  } else {
      useLPS22HB = false;
      Serial.println("Barometric sensor initialization failed!");
  }

  server.on ( "/ping", handlePing );
  server.on ( "/", handleData );
  server.onNotFound ( handleNotFound );
  server.begin(); 

  delay(500);

  // setupモードに入りやすくするための処理
  Serial.println("Wait for reconfigure.");
  String filename = configured_file;
  filename = filename + "tmp";
  SPIFFS.rename(configured_file, filename);
  showWaitForReconfig();
  SPIFFS.rename(filename, configured_file);
  Serial.println("reconfigure timeout. continue.");

  display.clear();
}

void loop_normal() {
  server.handleClient();

  char buf[16];
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(127, 0, "IP:" + WiFi.localIP().toString()); 
  display.drawString(127, 9, mDNS); 

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, product); 

  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 19, "T: " + String(lastTemp, 2) + " c" ); 
  display.drawString(0, 34, "H: " + String(lastHumidity, 2) + " %" ); 
  display.drawString(0, 49, "P: " + String(lastPressure, 1) + " hPa"); 
  display.display();

  readData();
  
  delay(2000);
}

void showStartupScreen() {
  display.init();

  if (doVerticalFlip) {
    display.flipScreenVertically();
  }
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0,  "ziomatrix corp.");
  display.drawString(0, 16, product_long);
  display.drawString(0, 32, "build " + build_date);
  display.drawString(0, 48, "initializing.");
  display.display();

  delay(1000);  
}

void showWifiInfo() {
  display.init();

//  String ssidStr = "SSID ";
//  ssidStr.concat(ssid);
  
  if (doVerticalFlip) {
    display.flipScreenVertically();
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, ssid);
  display.drawString(0, 16, WiFi.localIP().toString() );
  display.drawString(0, 32, mDNS);
  display.drawString(0, 48, "Starting up...");
  display.display();

  delay(1000);
   
}

void showWiFiErrorScreen() {
  // display init and show initial screen
  display.init();
  
  if (doVerticalFlip) {
    display.flipScreenVertically();
  }
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, product + " *HALT*");
  display.drawString(0, 16, "* Please Restart *");
  display.drawString(0, 32, "WiFi connect ERR");
  display.drawString(0, 48, "Check Settings");
  display.display();

}

void showWaitForReconfig() {
  display.init();
 
  if (doVerticalFlip) {
    display.flipScreenVertically();
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Wait for re-config");
  display.drawString(0, 32, "Power off now");
  display.drawString(0, 48, " to re-configure");

  int MAX_BAR = 25;
  for (int i = 0; i < MAX_BAR; i++) {
    String bar = "*";
    for(int j = 0; j < MAX_BAR; j++) {
      bar = bar + ( (j <= i) ? "-" : " ");
    }

    display.drawString(0, 16, bar);
    display.display();
    delay(250);
  }
   
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void handleData() {

  char buf[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  char temp[16], hum[16], pres[16];

  snprintf ( buf, sizeof buf,
    "{ \"uptime\": \"%02d:%02d:%02d\", \"uptimeMills\": \"%02d\", \"temparature\": \"%s\", \"humidity\": \"%s\", \"pressure\": \"%s\"}\n"
    , hr, min % 60, sec % 60, millis()
    , dtostrf(lastTemp, 0, 2, temp), dtostrf(lastHumidity, 0, 2, hum), dtostrf(lastPressure, 0, 2, pres)
  );
  server.send ( 200, F("application/json"), buf );
}

void handlePing() {

  char temp[100];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, sizeof temp,
    "{ \"uptime\": \"%02d:%02d:%02d\", \"uptimeMills\": \"%02d\" }\n"
    , hr, min % 60, sec % 60, millis()
  );
  server.send ( 200, F("application/json"), temp );
  // digitalWrite(BUZZER_PIN, LOW );
}

void readData() {
  float temp(NAN), hum(NAN), pres(NAN);

  // unit: B000 = Pa, B001 = hPa, B010 = Hg, B011 = atm, B100 = bar, B101 = torr, B110 = N/m^2, B111 = psi
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);
   
  // Parameters: (float& pressure, float& temp, float& humidity, bool celsius = false, uint8_t pressureUnit = 0x0)
  bme.read(pres, temp, hum, tempUnit, presUnit); 

  pres = pres / 100;
  
  Serial.print("Temp: ");
  Serial.print(temp, 2);
  Serial.print("C");
  Serial.print("\tHumidity: ");
  Serial.print(hum, 2);
  Serial.print("%");

  Serial.print("\tPressure (by BME): ");
  Serial.print(pres, 2);
  Serial.println("hPa");
  lastPressure = pres;

  if (useLPS22HB) {
    float tempPres(NAN);
    tempPres = lps22hb.readPressureHPA();
    if (tempPres != 0) {
      lastPressure = tempPres;      
      Serial.print("Pressure (by LPS22HB): ");
      Serial.print(tempPres, 2);
      Serial.println("hPa");
    }
  }

  lastTemp = temp;
  lastHumidity = hum;
}
