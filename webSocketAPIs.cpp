#include <WebServer.h>
#include <WebSocketsServer.h>
#include "webSocketAPIs.h"
#include <ArduinoJson.h>
#include "htmlPage.h"

// Use the HTTP server created in wifiService.cpp
#include "wifiService.h"
extern WebServer httpServer;

static WebSocketsServer ws(81);

static void handleRoot() {
  httpServer.send_P(200, "text/html", htmlPage);
}

static void sendState(uint8_t num) {
  bool active = getSystemState();
  bool rotationEnabled = getServoRotationEnabled();
  float speedMultiplier = getServoSpeedMultiplier();
  
  StaticJsonDocument<192> doc;
  doc["type"] = "state";
  doc["active"] = active;
  doc["rotationEnabled"] = rotationEnabled;
  doc["speedMultiplier"] = speedMultiplier;
  
  String response;
  serializeJson(doc, response);
  ws.sendTXT(num, response);
}

static void broadcastState() {
  bool active = getSystemState();
  bool rotationEnabled = getServoRotationEnabled();
  float speedMultiplier = getServoSpeedMultiplier();
  
  StaticJsonDocument<192> doc;
  doc["type"] = "state";
  doc["active"] = active;
  doc["rotationEnabled"] = rotationEnabled;
  doc["speedMultiplier"] = speedMultiplier;
  
  String response;
  serializeJson(doc, response);
  ws.broadcastTXT(response);
}

static void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {
  switch (type) {
    case WStype_CONNECTED:
      sendState(num);
      break;

    case WStype_TEXT: {
      // payload is NOT guaranteed null-terminated, so parse with length
      StaticJsonDocument<256> doc;  // adjust if your JSON is bigger

      DeserializationError err = deserializeJson(doc, payload, len);
      if (err) {
        Serial.print("WS JSON parse error: ");
        Serial.println(err.c_str());
        return;
      }
      
      const char* cmd = doc["command"];
      if (!cmd) return;

      Serial.print("Message received: ");
      Serial.println(cmd);

      if (strcmp(cmd, "toggle") == 0) {
        toggleSystemState();
        sendState(num);
      } else if (strcmp(cmd, "toggleRotation") == 0) {
        toggleServoRotation();
        sendState(num);
      } else if (strcmp(cmd, "setServoPosition") == 0) {
        if (doc.containsKey("angle")) {
          float angle = doc["angle"];
          setServoPosition(angle);
          // Broadcast updated position and state
          sendState(num);
          // Also broadcast a distance update with current position (even if system inactive)
          // This ensures the radar display updates when manually positioning
          broadcastDistance(-1, angle); // -1 means no distance reading, just position update
        }
      } else if (strcmp(cmd, "setServoSpeed") == 0) {
        if (doc.containsKey("multiplier")) {
          float multiplier = doc["multiplier"];
          setServoSpeedMultiplier(multiplier);
          // Broadcast updated state to all clients
          broadcastState();
        }
      } else if (strcmp(cmd, "getState") == 0) {
        sendState(num);
      }
      break;
    }

    default:
      break;
  }
}

void setupWebSocketAPI() {
  // HTTP routes
  httpServer.on("/", handleRoot);
  httpServer.on("/ping", [](){ httpServer.send(200, "text/plain", "pong"); });

  // WS
  ws.begin();
  ws.onEvent(onWsEvent);

  Serial.println("HTTP :80 and WS :81 ready");
}

void loopWebSocketAPI() {
  ws.loop();
}

void broadcastDistance(float distanceCm, float angle) {
  StaticJsonDocument<256> doc;
  doc["type"] = "distance";
  doc["angle"] = angle;
  
  if (distanceCm < 0) {
    doc["value"] = "no_reading";
    doc["distance"] = -1;
  } else {
    doc["value"] = String(distanceCm, 1) + " cm";
    doc["distance"] = distanceCm;
  }
  
  String response;
  serializeJson(doc, response);
  ws.broadcastTXT(response);
}
