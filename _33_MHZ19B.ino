#include "MHZ19.h"

bool WAIT_FOR_CO2_WARMUP = false;          // warmupを待つか否か。成功しないのでfalseでOK
int WAIT_FOR_CO2_SEC = 3;                  // warmupを待つ時間。↓がfalseの時のみ有効
bool WAIT_FOR_CO2_WARMUP_FOREVER = false;  // warmupが終わるまで永遠に待つ。恐らく無限ループになる

// 400ppmの校正(ABC)を行う。これをするには、20分以上外気に晒し続ける必要がある。
// 終了後は false に戻す。
bool AUTO_BASELINE_CORRECTION = false;

// MHZ19 serial (uart) pins
const int mhz_rx_pin = 12;  //Serial rx pin no
const int mhz_tx_pin = 14; //Serial tx pin no

//https://github.com/crisap94/MHZ19
MHZ19 *mhz19 = new MHZ19(mhz_rx_pin,mhz_tx_pin);

bool use_MHZ19 = true;

int lastPpm = -1;

void mhz_setup() {
  mhz19->begin(mhz_rx_pin, mhz_tx_pin);
  mhz19->setAutoCalibration(AUTO_BASELINE_CORRECTION);

  if (WAIT_FOR_CO2_WARMUP) {
    int wait = 0;

    while( mhz19->isWarming() ) {
      Serial.print(wait);
      Serial.print(" MH-Z19 now warming up...  status:");
      Serial.println(mhz19->getStatus());
      delay(1000);
      wait++;
      if ((wait > WAIT_FOR_CO2_SEC) || WAIT_FOR_CO2_WARMUP_FOREVER) {
        Serial.println("warmup timeout continue anyway");      
        break;
      }
    }
  }
}

void mhz_read_data() {

  if (use_MHZ19) {
    lastPpm = 0;

    if (!mhz19->isWarming() ) {
      // 恐らく、常に isWarming() == True.
      Serial.printf("warn: MH-Z19B: still warming up.");      
    }

    lastPpm = mhz19->getPPM(MHZ19_POTOCOL::UART);
    Serial.printf("MH-Z19B: PPM=");
    Serial.print(lastPpm);
    Serial.print("\n");

    // MQTT: if ppm == -1 , MH-Z19 error.
    if (lastPpm > 0) {
      char buf[24] = "";
      sprintf(buf, "%d", lastPpm);
    } else {
      lastPpm = -1;
    }
  }

  
}
