#include <Wire.h>

Tsl2561 Tsl(Wire);

bool use_tsl = false;

float lastLuxFull;
float lastLuxIr;

void tsl_setup() {
  // init TSL2561
  Tsl.begin();
  if( !Tsl.available() ) {
    Serial.println("No Tsl2561 found. Tsl2561 disabled.");
  }
}

void readDataTsl2561() {
  uint8_t id;
  uint16_t full, ir;

  Tsl.on();

  Tsl.setSensitivity(false, Tsl2561::EXP_402);
  //delay(16);
  //delay(110);
  delay(402 + 10);

  Tsl.id(id);
  Tsl.fullLuminosity(full);
  Tsl.irLuminosity(ir);

  Serial.printf("Tsl2561 at 0x%02x(id=0x%02x) luminosity is %5u (full) and %5u (ir)\n", Tsl.address(), id, full, ir);

  lastLuxFull = full;
  lastLuxIr = ir;

  Tsl.off(); 
}
