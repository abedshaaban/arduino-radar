#include <WebServer.h>
#include <WebSocketsServer.h>
#include "webSocketAPIs.h"
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
  String response = "{\"type\":\"state\",\"active\":";
  response += active ? "true" : "false";
  response += "}";
  ws.sendTXT(num, response);
}

static void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {
  if (type == WStype_CONNECTED) {
    // Send current state when client connects
    sendState(num);
  } else if (type == WStype_TEXT) {
    // Parse JSON message manually - create string with proper length
    char* msg = (char*)malloc(len + 1);
    if (msg) {
      memcpy(msg, payload, len);
      msg[len] = '\0';
      String message = String(msg);
      message.trim();
      free(msg);
      
      // Check for toggle command
      if (message.indexOf("\"command\":\"toggle\"") >= 0) {
        // Toggle the state and send back the new state
        toggleSystemState();
        sendState(num); // Send updated state back as response
      } else if (message.indexOf("\"command\":\"getState\"") >= 0) {
        // Client requests current state (optional - state is sent on connect)
        sendState(num);
      }
    }
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

void broadcastDistance(float distanceCm) {
  String response = "{\"type\":\"distance\",\"value\":\"";
  if (distanceCm < 0) {
    response += "no_reading";
  } else {
    response += String(distanceCm, 1) + " cm";
  }
  response += "\"}";
  ws.broadcastTXT(response);
}
