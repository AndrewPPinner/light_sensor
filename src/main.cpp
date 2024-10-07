
#include <BH1750.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

BH1750 lightMeter(0x23);

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

void setup() {

  Serial.begin(115200);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void loop() {
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    //Send data to database
    delay(600000);
  }
}