#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/**
 * 初期化
 */
void setup_setupmode() {
  // ファイルシステム初期化
  setup_server();

  showSetupStartupScreen();
}

void loop_setupmode() {
  server.handleClient();
}

/**
 * WiFi設定
 */
void handleRootGet() {
  String html = "";
  html += "<html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<style>";
  html += "  input { width:200px; }";
  html += "</style>";
  html += "</head>";
  html += "<h1>" + product + " Settings</h1>";
  html += "<form method='post'>";
  html += "  <br>";
  html += "  以下の設定は、接続先WiFiの情報です。面倒ですが正しく設定して下さい。<br>";
  html += "  2.4GHz帯のみ対応しています。<br>";
  html += "  <input type='text' name='ssid' placeholder='ssid' autofocus><br>";
  html += "  <input type='text' name='pass' placeholder='pass'><br>";
  html += "  以下の設定は、mDNS(Rendezvous) の名前です。LAN内の他の端末等と重複しないようにして下さい。<br>";
  html += "  ハイフン、アンダースコアを使用すると名前解決に失敗する可能性があるため非推奨です。<br>";
  html += "  <input type='text' name='mdnsname' placeholder='mdnsname' value='" + ssid + "'><br>";
  html += "  画面表示を上下反転するか否かの設定です。今見えている画面が正常であれば、1、そうでなければ 0 を設定して下さい。<br>";
  html += "  <input type='text' name='vflip' placeholder='1 = yes 0 = no' value=''><br>";
  html += "  <br>";
  html += "  <input type='submit' value='設定する'><br>";
  html += "</form>";
  html += "<br>";
  html += "<hr>";
  html += product_long + ", Copyright 2018-2019 ziomatrix.org.";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleRootPost() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  String mdnsname = server.arg("mdnsname");

  SPIFFS.begin();

  // 設定ファイル
  File f = SPIFFS.open(settings, "w");
  f.println(ssid);
  f.println(pass);
  f.println(mdnsname);
  f.close();

  // 設定済みフラグファイル
  File f2 = SPIFFS.open(configured_file, "w");
  f2.println("ok");
  f2.close();

  String html = "";
  html += "<html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<style>";
  html += "  input { width:200px; }";
  html += "</style>";
  html += "</head>";

  html += "<h1>" + product + " setting done</h1>";
  html += "SSID " + ssid + "<br>";
  html += "PASS " + pass + "<br>";
  html += "mDNS " + mdnsname + "<br>";
  html += "<br>";
  html += "Restart (power off then power on) to use above setting.<br>";
  html += "設定が完了しました。リセットボタンを押して再起動して下さい。<br>";
  html += "<br>";
  html += "<a href='/'>setting again</a>";
  html += "</html>";
  server.send(200, "text/html", html);
}

/**
 * 初期化(サーバモード)
 */
void setup_server() {
  byte mac[6];
  WiFi.macAddress(mac);

  // SSID は eb3macaddress
  ssid = product_short;
  for (int i = 0; i < 6; i++) {
    ssid += String(mac[i], HEX);
  }
  
  Serial.println("SSID: " + ssid);
  // Serial.println("PASS: " + pass);

  /* You can remove the password parameter if you want the AP to be open. */
  // WiFi.softAP(ssid.c_str(), pass.c_str());
  WiFi.softAP(ssid.c_str());

  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, handleRootPost);
  server.begin();
  Serial.println("HTTP server started.");
}

void showSetupStartupScreen() {
  // display init and show initial screen
  display.init();
  
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0,  product_long);
  display.drawString(0, 16, "Setup mode.");
  display.drawString(0, 33, "http://" + WiFi.softAPIP().toString() + "/" );
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 52, ssid);
  display.display();

  delay(1000);  
}
