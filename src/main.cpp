
#include <BH1750.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Creds.h>
#include <ArduinoJson.h>

BH1750 lightMeter(0x23);
const char* _serverName = "https://andrew-pinner.asuscomm.com/smart/api/lightSensor";
const char* _authServer = "https://andrewp.online/RCON/api/login";
String _token = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void GetAuth() {
  String token;

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;
    https.useHTTP10(true);

    if (https.begin(client, _authServer))
    {
      String request;
      JsonDocument doc;
      doc["Username"] = Username;
      doc["Password"] = Password;

      serializeJson(doc, request);

      https.addHeader("Content-Type", "application/json");
      int httpCode = https.POST(request);

      JsonDocument response;
      deserializeJson(response, https.getStream());
      token = response["Token"].as<String>();

      https.end();
    }
    else
    {
      Serial.println("Could not connect to server");
    }
  }

  _token = token;
}

void UpdateSensor() {
  float lux = lightMeter.readLightLevel();
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.useHTTP10(true);

  if (_token.isEmpty())
  {
    GetAuth();
  }
  
  if (https.begin(client, _serverName))
  {
    JsonDocument doc;
    doc["Location"] = "Cilantro";
    doc["SensorValue"] = lux;
    doc["SensorType"] = "Light_Sensor";

    String request;
    serializeJson(doc, request);
    Serial.println(request);

    https.addHeader("Content-Type", "application/json");
    https.addHeader("Authorization", "Bearer " + _token);
    int httpCode = https.POST(request);
    https.end();

    if (httpCode == 401)
    {
      _token = "";
      UpdateSensor();
    }
    
  }
  else
  {
    Serial.println("Could not connect to server");
  }
}

void loop()
{
  if (lightMeter.measurementReady())
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      UpdateSensor();
    }
    delay(600000);
  }
}