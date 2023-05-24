#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoHttpClient.h>

#define BLYNK_TEMPLATE_ID "TMPL6bA8UYqJE"
#define BLYNK_TEMPLATE_NAME "Smart Trash Bin"
#define BLYNK_AUTH_TOKEN "vOBF__Yp6UZTo8-jH2VQeGWPnbxb8Qb6"

#define WIFI_SSID "Wifi Free 2 :)"
#define WIFI_PASS "wifin0tfree"

#define THINGSPEAK_SERVER "http://api.thingspeak.com"
#define THINGSPEAK_API_KEY "Y8QRMNLU2MP0HLNM"

#define SEND_DATA_INTERVAL 10000 // millisecond

long lastSendTime = 0; // millisecond
int openTimes = 0;

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();

  Serial.println(millis() - lastSendTime);
  
  if (millis() - lastSendTime > SEND_DATA_INTERVAL) {
    sendData();
    lastSendTime = millis();
    openTimes = 0;
  }
  
  int s = Serial.read();
  if (s == 42) {
    openTimes++;
  }
}

void sendData() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, String(THINGSPEAK_SERVER) + "/update");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST("field1=" + String(openTimes) + "&key=" + THINGSPEAK_API_KEY);

  if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  
  http.end();
}

BLYNK_WRITE(V0)  {
  String pinValue = param.asStr();
  Serial.println("BlockLid " + pinValue + "\r\n");
}

BLYNK_WRITE(V1) {
  String pinValue = param.asStr();
  Serial.println("KeepOpenTime " + pinValue + "\r\n");
}
