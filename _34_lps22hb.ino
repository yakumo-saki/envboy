// using sodaq_lps22hb ver 1.0.0
#include <Sodaq_LPS22HB.h>

bool use_LPS22HB = false;

Sodaq_LPS22HB lps22hb;

extern float lastPressure;

void lps22hb_setup() {
  // init LPS22HB if found (0x5d)
  if (lps22hb.init()) {
      Serial.println("Barometric sensor initialization succeeded!");
      lps22hb.enableSensor(Sodaq_LPS22HB::OdrOneShot);
      use_LPS22HB = true;
  } else {
      use_LPS22HB = false;
      Serial.println("Barometric sensor initialization failed!");
  }

}

void lps22hb_read_data() {
  
  if (use_LPS22HB) {
    // LPS22HBを使う場合は、 pres を上書きする
    float tempPres(NAN);
    tempPres = lps22hb.readPressureHPA();
    lps22hb.disableSensor();
    
    if (tempPres != 0) {
      lastPressure = tempPres;      
      Serial.print("Pressure (by LPS22HB): ");
      Serial.print(tempPres, 2);
      Serial.println("hPa");
    }  
  }

}
