#include "MHZ19.h"
#include <SoftwareSerial.h>

const bool WAIT_FOR_CO2_WARMUP = false;          // warmupを待つか否か。成功しないのでfalseでOK
const int  WAIT_FOR_CO2_SEC = 3;                  // warmupを待つ時間。↓がfalseの時のみ有効
const bool WAIT_FOR_CO2_WARMUP_FOREVER = false;  // warmupが終わるまで永遠に待つ。恐らく無限ループになる

// 400ppmの校正(ABC)を行う。これをするには、20分以上外気に晒し続ける必要がある。
// 終了後は false に戻す。
const bool AUTO_BASELINE_CORRECTION = false;

// MHZ19 serial (uart) pins
const int MHZ_RX_PIN = 12;  //Serial rx pin no
const int MHZ_TX_PIN = 14; //Serial tx pin no

MHZ19 mhz19;
SoftwareSerial softSerial(MHZ_RX_PIN, MHZ_TX_PIN);

bool use_MHZ19 = true;
unsigned long data_timer = 0;

int lastPpm = -1;

void mhz_setup() {

  softSerial.begin(9600);   // MH-Z19B's serial is 9600 baud
  mhz19.begin(softSerial);
  mhz19.autoCalibration();  // On にしても常に400になることはない

  // firmware version
  char myVersion[4];
  mhz19.getVersion(myVersion);
  for(byte i = 0; i < 4; i++)
  {
    Serial.print(myVersion[i]);
    if(i == 1)
      Serial.print(".");    
  }
  Serial.println("");
 
  // firmware version
  Serial.print("Range: ");
  Serial.println(mhz19.getRange());   
  Serial.print("Background CO2: ");
  Serial.println(mhz19.getBackgroundCO2());
  Serial.print("Temperature Cal: ");
  Serial.println(mhz19.getTempAdjustment());   
}

void mhz_read_data() {
  // Check if interval has elapsed (non-blocking delay() equivilant)
  if (millis() - data_timer >= 2000) {
    mhz_read_data_impl();
    data_timer = millis(); 
  }
}

void mhz_read_data_impl() {
    if (use_MHZ19) {

    lastPpm = 0;

    lastPpm = mhz19.getCO2(false);
    int temp = mhz19.getTemperature();
    
    Serial.printf("MH-Z19B: PPM=");
    Serial.print(lastPpm);
    Serial.print(" Temparature=");
    Serial.print(temp);
    Serial.print("\n");

    // if ppm == -1 or 0 , MH-Z19 error.
    if (lastPpm < 1) {
      Serial.println("MH-Z19B: Maybe error. value set to -1");
      lastPpm = -1;
    }
  }
}
