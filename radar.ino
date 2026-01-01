#include <ESP32Servo.h>
#include "wifiService.h"
#include "webSocketAPIs.h"
#include "ultrasonicService.h"

// WIFI Credentials
const char* WIFI_SSID = "RADAR-67";
const char* WIFI_PASS = "67676767";

const int TRIG_PIN = 18;
const int ECHO_PIN = 19;

#define SERVO_PIN 5

Servo servo;

// System state - all logic and state management in radar.ino
bool systemActive = false;

// Servo rotation state (non-blocking)
float servoPosition = 0;
int servoDirection = 1; // 1 for forward (0->180), -1 for backward (180->0)
unsigned long lastServoMove = 0;
const unsigned long SERVO_MOVE_INTERVAL = 20; // milliseconds between servo moves
const float SERVO_STEP = 3; // degrees per step

// Functions for WebSocket API to access system state
bool getSystemState() {
  return systemActive;
}

bool toggleSystemState() {
  systemActive = !systemActive;
  Serial.print("System state toggled to: ");
  Serial.println(systemActive ? "ACTIVE" : "INACTIVE");
  return systemActive;
}

void setup() {
  Serial.begin(115200);

  servo.attach(SERVO_PIN);
  servo.write(0);

  registerUltrasonicSensor(TRIG_PIN, ECHO_PIN);
  setupWifiAccessPoint(WIFI_SSID, WIFI_PASS);
  setupWebSocketAPI(); // sets up HTTP routes + WebSocket
}

void loop() {
  loopWifiAccessPoint();
  loopWebSocketAPI();

  if(systemActive){
    rotateServo();

    // read the distance and broadcast
    static unsigned long last = 0;
    if (millis() - last >= 300) {
      last = millis();
      float d = getDistanceInCm();
      broadcastDistance(d);
    }
  }
}

void rotateServo() {
  // Non-blocking servo rotation - only moves one step per call
  unsigned long now = millis();
  if (now - lastServoMove >= SERVO_MOVE_INTERVAL) {
    lastServoMove = now;
    
    // Update position based on direction
    servoPosition += servoDirection * SERVO_STEP;
    
    // Reverse direction at limits
    if (servoPosition >= 180) {
      servoPosition = 180;
      servoDirection = -1;
    } else if (servoPosition <= 0) {
      servoPosition = 0;
      servoDirection = 1;
    }
    
    // Move servo to new position
    servo.write((int)servoPosition);
  }
}