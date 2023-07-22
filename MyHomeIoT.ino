#include <ESP8266WiFi.h>
#include "Credentials.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include <ESP8266WebServer.h>

// Create servo objects for each servo motor
Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;

ESP8266WebServer server(80); // Create a web server on port 80


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

   server.on("/servo", []() {
    servo0.write(90); // Adjust angle as needed
    delay(1000);
    servo0.write(0);
    server.send(200, "text/plain", "Servo moved");
  });
  server.begin();

  // Attach servos to their respective pins
  servo0.attach(16); 
  servo1.attach(5);
  servo2.attach(4);
  servo3.attach(0);
  servo4.attach(2);
  servo5.attach(14);
  servo6.attach(12);
  servo7.attach(13);
}

void loop() {
  for (int pos = 0; pos <= 180; pos += 20) { // goes from 0 degrees to 180 degrees
    // in steps of 20 degrees
    servo0.write(pos); // tell servo to go to position in variable 'pos'
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    servo4.write(pos);
    servo5.write(pos);
    servo6.write(pos);
    servo7.write(pos);
    delay(1000); // waits 1 sec for the servo to reach the position
  }
  for (int pos = 180; pos >= 0; pos -= 20) { // goes from 180 degrees to 0 degrees
    servo0.write(pos);
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    servo4.write(pos);
    servo5.write(pos);
    servo6.write(pos);
    servo7.write(pos);
    delay(1000);
  }
}
