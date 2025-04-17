#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>

// Pins
#define METAL_EAST 34
#define METAL_RST 12
#define METAL_WEST 35
#define NEOPIXEL_EAST 15
#define NEOPIXEL_WEST 4
#define SERVO_PIN 13

// NeoPixels
Adafruit_NeoPixel pixelEast(1, NEOPIXEL_EAST, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelWest(1, NEOPIXEL_WEST, NEO_GRB + NEO_KHZ800);

// Servo
Servo camServo;

// Wi-Fi credentials
const char* ssid = "Galaxy";
const char* password = "qwerty123";

// Web server on port 80
WebServer server(80);

// Vehicle count from laptop ML
int trafficEast = 0;
int trafficWest = 0;

// Green signal duration (in ms)
const unsigned long greenDuration = 15000;

void setup() {
  Serial.begin(115200);

  pinMode(METAL_EAST, INPUT);
  pinMode(METAL_WEST, INPUT);

  pinMode(METAL_RST, OUTPUT);

  pixelEast.begin();
  pixelWest.begin();
  pixelEast.setBrightness(50);
  pixelWest.setBrightness(50);
  pixelEast.show();
  pixelWest.show();

  camServo.attach(SERVO_PIN);
  camServo.write(90); // Center position

  connectToWiFi();
  setupServer();
}

void loop() {
  digitalWrite(METAL_RST, HIGH);
  server.handleClient();

  // Check metal detectors
  bool eastActive = analogRead(METAL_EAST) > 3000;
  bool westActive = analogRead(METAL_WEST) > 3000;

  if (eastActive && !westActive) {
    handleSingleDirection("east");
    digitalWrite(METAL_RST, HIGH);
  } else if (westActive && !eastActive) {
    handleSingleDirection("west");
    digitalWrite(METAL_RST, HIGH);
  } else if (eastActive && westActive) {
    handleBothDirections();
    digitalWrite(METAL_RST, HIGH); // Use ML
  }

  delay(1000);
}

// -------------------- FUNCTIONS --------------------

void handleSingleDirection(String dir) {
  showSignal(dir, "yellow");
  delay(3000);
  showGreenFor(dir);
}

void handleBothDirections() {
  rotateCam("east");
  delay(3000);
  rotateCam("west");
  delay(3000);
  camServo.write(90); // Reset to center

  // Let ML (laptop) decide
  Serial.println("Waiting for vehicle count from ML...");
  // Assume POST will update trafficEast and trafficWest
}

void showSignal(String dir, String color) {
  if (dir == "east") {
    if (color == "red") pixelEast.setPixelColor(0, pixelEast.Color(255, 0, 0));
    else if (color == "yellow") pixelEast.setPixelColor(0, pixelEast.Color(255, 255, 0));
    else if (color == "green") pixelEast.setPixelColor(0, pixelEast.Color(0, 255, 0));
    pixelEast.show();
  } else if (dir == "west") {
    if (color == "red") pixelWest.setPixelColor(0, pixelWest.Color(255, 0, 0));
    else if (color == "yellow") pixelWest.setPixelColor(0, pixelWest.Color(255, 255, 0));
    else if (color == "green") pixelWest.setPixelColor(0, pixelWest.Color(0, 255, 0));
    pixelWest.show();
  }
}

void showGreenFor(String dir) {
  showSignal(dir, "green");
  Serial.println(dir + " is GREEN for 15 seconds.");
  unsigned long start = millis();
  while (millis() - start < greenDuration) {
    delay(500);
  }
  showSignal(dir, "red");
  Serial.println(dir + " green phase ended.");
}

void rotateCam(String dir) {
  if (dir == "east") {
    camServo.write(135);
    Serial.println("Rotating camera EAST");
  } else if (dir == "west") {
    camServo.write(45);
    Serial.println("Rotating camera WEST");
  }
}

// -------------------- Wi-Fi + Web Server --------------------

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());
}

void setupServer() {
  server.on("/vehiclecount", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      // Expecting format: "east,west" → e.g., "3,5"
      int commaIndex = body.indexOf(',');
      if (commaIndex != -1) {
        trafficEast = body.substring(0, commaIndex).toInt();
        trafficWest = body.substring(commaIndex + 1).toInt();
        Serial.println("Received counts: East = " + String(trafficEast) + ", West = " + String(trafficWest));

        // Prioritize green direction
        if (trafficEast > trafficWest) {
          handleSingleDirection("east");
        } else if (trafficWest > trafficEast) {
          handleSingleDirection("west");
        } else {
          // Tie: default to east
          handleSingleDirection("east");
        }
        server.send(200, "text/plain", "Counts received.");
      } else {
        server.send(400, "text/plain", "Bad format");
      }
    } else {
      server.send(400, "text/plain", "No data received");
    }
  });

  server.begin();
  Serial.println("HTTP server started.");
}
