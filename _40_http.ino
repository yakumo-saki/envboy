void server_setup() {
  server.on ( "/ping", handlePing );
  server.on ( "/", handleData );
  server.onNotFound ( handleNotFound );
  server.begin(); 
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
  char lux[16], luxIr[16];

  snprintf ( buf, sizeof buf,
    "{ \"uptime\": \"%02d:%02d:%02d\", \"uptimeMills\": \"%02d\", \"temparature\": \"%s\", \"humidity\": \"%s\", \"pressure\": \"%s\",\"luminous\": \"%s\", \"luminousIr\": \"%s\" }"
    , hr, min % 60, sec % 60, millis()
    , dtostrf(lastTemp, 0, 2, temp), dtostrf(lastHumidity, 0, 2, hum), dtostrf(lastPressure, 0, 2, pres)
    , dtostrf(lastLuxFull, 0, 0, lux), dtostrf(lastLuxIr, 0, 0, luxIr)
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
