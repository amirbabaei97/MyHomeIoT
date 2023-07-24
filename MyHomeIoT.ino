#include <ESP8266WiFi.h>
#include "Credentials.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <WiFiClient.h>

WiFiClient client;

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

bool checkAuth() {
    return server.authenticate(www_username, www_password);
}
void handleRoot() {
  server.send(200, "text/plain", "You are logged in");
}


  void sendStateToAPI(String endpoint, bool state) {
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure(); // Ignore SSL certificate validation

    HTTPClient https;
    
    String url = "https://x8ki-letl-twmt.n7.xano.io/api:QgTMv-fR" + endpoint;
    if (https.begin(*client, url)) {  // HTTPS
      https.addHeader("Content-Type", "application/json");
      String jsonBody = "{\"State\":" + String(state ? "true" : "false") + "}";
       Serial.println("url: " + url);
      Serial.println("json to send: " + jsonBody);
      int httpCode = https.POST(jsonBody);

      if (httpCode > 0) {
        String payload = https.getString();
        Serial.println("http code: " + httpCode);
        Serial.println("Response: " + payload);
      } else {
        Serial.printf("Error on HTTPS POST: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.println("HTTPS Connection Failed");
    }
  } else {
    Serial.println("Not connected to WiFi");
  }
}

void setup() {
  Serial.begin(115200);

  // Attach servos to their respective pins
  servo0.attach(16); 
  servo1.attach(5);
  servo2.attach(4);
  servo3.attach(0);
  servo4.attach(2);
  servo5.attach(14);
  servo6.attach(12);
  servo7.attach(13);

  // Connect to WiFi, print IP address, etc.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setupServoRoutes(); // Setup web server routes for controlling the servos
  server.begin(); // Start the web server
  Serial.println("HTTP server started");
  server.on("/", HTTP_GET, []() {
    if (!checkAuth()) return;
    handleRoot();
  });
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}

// Function to control a specific servo
// servoNumber: the number of the servo to control
// turnOn: whether to turn the servo to the 'on' position or 'off' position
void controlServo(int servoNumber, bool turnOn) {
  //These values can(should) be tweaked for the servos using the other tweak.ino file in the project. 
  int onPosition = 20; // 'On' position for the servo
  int offPosition = 160; // 'Off' position for the servo
  int restPosition = 90; // Rest position for the servo

  Servo* servo; // Pointer to the servo to control
  switch (servoNumber) {
    case 0: servo = &servo0; break;
    case 1: servo = &servo1; break;
    case 2: servo = &servo2; break;
    case 3: servo = &servo3; break;
    case 4: servo = &servo4; break;
    case 5: servo = &servo5; break;
    case 6: servo = &servo6; break;
    case 7: servo = &servo7; break;
    default: return; // Exit if an invalid servo number is given
  }

  servo->write(turnOn ? onPosition : offPosition); // Move servo to 'on' or 'off' position
  delay(200);
  servo->write(restPosition); // Return servo to rest position
}

// Function to control the lamp servos
// lampNumber: the specific lamp number
// turnOn: whether to turn the lamp on or off
void lamp(int lampNumber, bool turnOn) {
    controlServo(lampNumber, turnOn);
    // Sending state to API
    sendStateToAPI("/lamp" + String(lampNumber), turnOn);
}

// Function to control the blinds
// blindNumber: the number of the blind (1 or 2)
// lowerBlinds: whether to lower or raise the blinds
void blinds(int blindNumber, bool lowerBlinds) {
    if (blindNumber == 1) {
        if (lowerBlinds) {
            // Blind 1 Going Down
            controlServo(5, true); // Turn off servo 5
            controlServo(3, true);  // Turn on servo 3
        } else {
            // Blind 1 Going Up
            controlServo(3, false); // Turn off servo 3
            controlServo(5, false);  // Turn on servo 5
        }
    } else if (blindNumber == 2) {
        if (lowerBlinds) {
            // Blind 2 Going Down
            controlServo(6, true); // Turn off servo 6
            controlServo(4, true);  // Turn on servo 4
        } else {
            // Blind 2 Going Up
            controlServo(4, false); // Turn off servo 4
            controlServo(6, false);  // Turn on servo 6
        }
    }
  // Sending state to API
  String endpoint = "/blinds" + String(blindNumber);
  sendStateToAPI(endpoint, lowerBlinds);
}

// Function to control the AC
void acControl() {
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure(); // Ignore SSL certificate validation

    HTTPClient https;
    String urlGet = "https://x8ki-letl-twmt.n7.xano.io/api:QgTMv-fR/ac";

    // GET Request
    if (https.begin(*client, urlGet)) {
      int httpCodeGet = https.GET();

      if (httpCodeGet > 0) {
        String payload = https.getString();
        Serial.println("GET Response: " + payload);
        
        // Parse JSON response and toggle state
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        bool currentState = doc["State"];
        bool newState = !currentState;
        controlServo(7, false); // Control AC using servo 7

        https.end(); // End the GET request

        // Delay before POST request
        delay(500);

        // POST Request
        String urlPost = "https://x8ki-letl-twmt.n7.xano.io/api:QgTMv-fR/ac";
        if (https.begin(*client, urlPost)) {
          https.addHeader("Content-Type", "application/json");
          String jsonBody = "{\"State\":" + String(newState ? "true" : "false") + "}";
          int httpCodePost = https.POST(jsonBody);

          if (httpCodePost > 0) {
            String response = https.getString();
            Serial.println("POST Response: " + response);
          } else {
            Serial.printf("Error on HTTPS POST: %s\n", https.errorToString(httpCodePost).c_str());
          }

          https.end(); // End the POST request
        } else {
          Serial.println("HTTPS POST Connection Failed");
        }
      } else {
        Serial.printf("Error on HTTPS GET: %s\n", https.errorToString(httpCodeGet).c_str());
      }

    } else {
      Serial.println("HTTPS GET Connection Failed");
    }
  } else {
    Serial.println("Not connected to WiFi");
  }
}

// Function to set up web server routes for controlling servos
void setupServoRoutes() {
  // Routes for controlling all lamps
  server.on("/lamps/on", []() {
    if (!checkAuth()) return;
    for (int i = 0; i <= 2; i++) { // For lamps 0, 1, and 2
        lamp(i, true); // Turn on each lamp
    }
    server.send(200, "text/plain", "All lamps turned on");
  });
  
  server.on("/lamps/off", []() {
    if (!checkAuth()) return;
    for (int i = 0; i <= 2; i++) { // For lamps 0, 1, and 2
        lamp(i, false); // Turn off each lamp
    }
    server.send(200, "text/plain", "All lamps turned off");
  });

  // Routes for controlling individual lamps
  for (int i = 0; i <= 2; i++) { // Only for lamps 0, 1, and 2
    server.on("/lamp" + String(i) + "/on", [i]() {
        if (!checkAuth()) return;
        lamp(i, true);
        server.send(200, "text/plain", "Lamp " + String(i) + " turned on");
    });
    server.on("/lamp" + String(i) + "/off", [i]() {
        if (!checkAuth()) return;
        lamp(i, false);
        server.send(200, "text/plain", "Lamp " + String(i) + " turned off");
    });
}


  // Routes for controlling blinds
  server.on("/blinds1/down", []() {
    if (!checkAuth()) return;
    blinds(1, true);
    server.send(200, "text/plain", "Blinds 1 lowered");
  });
  server.on("/blinds1/up", []() {
    if (!checkAuth()) return;
    blinds(1, false);
    server.send(200, "text/plain", "Blinds 1 raised");
  });
  server.on("/blinds2/down", []() {
    if (!checkAuth()) return;
    blinds(2, true);
    server.send(200, "text/plain", "Blinds 2 lowered");
  });
  server.on("/blinds2/up", []() {
    if (!checkAuth()) return;
    blinds(2, false);
    server.send(200, "text/plain", "Blinds 2 raised");
  });

  // Route for AC control
  server.on("/ac/toggle", []() {
    if (!checkAuth()) return;
    acControl();
    server.send(200, "text/plain", "AC toggle command sent");
  });
}