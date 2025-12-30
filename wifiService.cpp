#include <WiFi.h>
#include <WebServer.h>
#include "wifiService.h"

WebServer httpServer(80); // shared server (API file will add routes)

void setupWifiAccessPoint(const char* ssid, const char* pass) {
  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(ssid, pass);

  Serial.println("\n\n\n==============================");
  Serial.println(ok ? "Hotspot started" : "Hotspot FAILED");
  Serial.print("AP SSID: "); Serial.println(ssid);
  Serial.print("AP IP: ");   Serial.println(WiFi.softAPIP().toString());
  Serial.println("============================== \n\n\n");

  httpServer.begin();
}

void loopWifiAccessPoint() {
  httpServer.handleClient();
}
