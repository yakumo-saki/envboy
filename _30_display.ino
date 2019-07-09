// 最後に取得した温度
extern float lastTemp;
extern float lastHumidity;
extern float lastPressure;
extern float lastLuxFull;
extern float lastLuxIr;
extern int lastPpm;

int display_last_ppm;  // センサーがエラーの時でも値をホールドする変数

int MAX_BAR = 22; // reconfigure 待ちの bar本数

void showStartupScreen(String product_long) {
  display.init();

  if (needFlip) {
    display.flipScreenVertically();
  }
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0,  "ziomatrix corp.");
  display.drawString(0, 16, product_long);
  display.drawString(0, 32, "initializing.");
  display.drawString(0, 48, "Please wait");
  display.display();

  delay(1000);  
}

void showWifiInfo(String ip, String mDNS) {
  display.init();
 
  if (needFlip) {
    display.flipScreenVertically();
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, ssid);
  display.drawString(0, 16, ip);
  display.drawString(0, 32, mDNS);
  display.drawString(0, 48, "Starting up...");
  display.display();

  delay(1000);
   
}

void showWiFiErrorScreen() {
  // display init and show initial screen
  display.init();
  
  if (needFlip) {
    display.flipScreenVertically();
  }
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, product + " *HALT*");
  display.drawString(0, 16, "* Please Restart *");
  display.drawString(0, 32, "WiFi connect err");
  display.drawString(0, 48, "Check Settings");
  display.display();

}

void showWaitForReconfig() {
  display.init();
 
  if (needFlip) {
    display.flipScreenVertically();
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Wait for re-config");
  display.drawString(0, 32, "Power off now");
  display.drawString(0, 48, " to re-configure");

  for (int i = 0; i < MAX_BAR; i++) {
    String bar = "*";
    for(int j = 0; j < MAX_BAR; j++) {
      bar = bar + ( (j <= i) ? "-" : " ");
    }

    display.drawString(0, 16, bar);
    display.display();
    delay(150);
  }
   
}

void show_main(String ip, String mdns) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(127, 0, "IP:" + ip); 
  display.drawString(127, 9, mDNS); 

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, product); 

  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 19, "T:" + String(lastTemp, 1) + "c" + "  " + "H:" + String(lastHumidity, 1) + "%" ); 

  // LINE2
  String line2 = "P:" + String(lastPressure, 1);
  if (lastLuxFull >= 0) {
    line2 = line2 + " " + "L:" + String(lastLuxFull, 0) + "lx";
  } else {
    line2 = line2 + " " + "L: -";    
  }
  display.drawString(0, 34, line2);

  if (lastPpm > 0) {
    display.drawString(0, 49, String("CO2:") + String(lastPpm) + "ppm" );
    display_last_ppm = lastPpm;
  } else {
    if (display_last_ppm > 0) {
      display.drawString(0, 49, String("CO2:") + "** " + String(display_last_ppm) + "ppm" );
    } else {
      display.drawString(0, 49, String("CO2:") + "-" );
    }
  }

  display.display();

}
