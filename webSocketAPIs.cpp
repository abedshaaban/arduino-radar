#include <WebServer.h>
#include <WebSocketsServer.h>
#include "webSocketAPIs.h"

// Use the HTTP server created in wifiService.cpp
#include "wifiService.h"
extern WebServer httpServer;

static WebSocketsServer ws(81);

static void handleRoot() {
  const char* page =
    "<!doctype html><html><body>"
    "<h2>ESP32 Distance</h2>"
    "<div id='d'>---</div>"
    "<script>"
    "let ws=new WebSocket('ws://'+location.hostname+':81/');"
    "ws.onmessage=e=>{document.getElementById('d').innerText=e.data;};"
    "</script>"
    "</body></html>";
  httpServer.send(200, "text/html", page);
}

static void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {
  if (type == WStype_CONNECTED) {
    ws.sendTXT(num, "connected");
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
  if (distanceCm < 0) {
    ws.broadcastTXT("no_reading");
  } else {
    // send a simple text value; can switch to JSON later
    ws.broadcastTXT(String(distanceCm, 1) + " cm");
  }
}
