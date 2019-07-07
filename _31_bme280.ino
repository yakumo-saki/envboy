BME280I2C bme;   // Default : forced mode, standby time = 1000 ms
                 // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
                 // Address 0x76

static bool use_bme = false;

float lastTemp;
float lastHumidity;
float lastPressure;

void bme_setup() {
  // init BME
  while(!bme.begin()){
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }
}

void readDataBme280() {
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
  Serial.print("\t\tHumidity: ");
  Serial.print(hum, 2);
  Serial.print("%");
  Serial.print("\t\tPressure: ");
  Serial.print(pres, 2);
  Serial.println("hPa");

  lastTemp = temp;
  lastHumidity = hum;
  lastPressure = pres;
}
