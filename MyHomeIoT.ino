#include <ESP8266WiFi.h>
#include "Credentials.h"

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

//Testing a simple http request
if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://www.google.com");
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }
    http.end();
  }
}

void loop() {
}
