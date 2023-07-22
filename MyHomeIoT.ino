#include <ESP8266WiFi.h>
#include "Credentials.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>

Servo servo0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//Testing a HTTPs request
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    https.begin(*client, "https://x8ki-letl-twmt.n7.xano.io/api:QgTMv-fR");
    int httpCode = https.GET();
    if (httpCode > 0) {
      String payload = https.getString();
      Serial.println(payload);
    }
    https.end();
  }
  // Attach servos to their respective pins
  servo0.attach(16);
}

void loop() {
  servo0.write(180); //play around with the velue to get the right angle - mine is 150 for TowerPro SG90
  delay(1000);
  servo0.write(0);
  delay(1000);
}
