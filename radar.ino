#include <WiFi.h>
#include <WebServer.h>

// WIFI Credentials
const char* apSSID = "RADAR-67";
const char* apPASS = "";   // must be 8+ chars (or use "" for open network)


// run the web server on port 80
WebServer server(80);

void handleRoot() {
  server.send(200, "text/html",
              "<h1>ESP32-S3 AP Web Server</h1>"
              "<p>If you can see this, it works.</p>");
}

const int TRIG_PIN = 18;
const int ECHO_PIN = 19;

void setup() {
  Serial.println("\n\n\n==============================");
  Serial.println("ESP32-S3 BOOT");
  Serial.println("============================== \n\n\n");

  // unltra sonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(115200);

  // Start Access Point (hotspot)
  WiFi.mode(WIFI_AP);

  // Web server routes
  server.on("/", handleRoot);
  server.on("/ping", []() { server.send(200, "text/plain", "pong"); });

  server.begin();
  Serial.println("Web server started on port 80");

  bool ok = WiFi.softAP(apSSID, apPASS);

  Serial.println();
  Serial.println(ok ? "Hotspot started!" : "Hotspot start FAILED!");

  Serial.println("AP SSID: " + String(apSSID));
  Serial.println("AP PASS: " + String(strlen(apPASS) > 0 ? apPASS : "-"));
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP().toString());  // usually 192.168.4.1
}

void loop() {
  server.handleClient();



  // float d = getDistance();
  // if (d < 0) Serial.println("No reading");
  // else {
  //   Serial.print("Distance: ");
  //   Serial.print(d);
  //   Serial.println(" cm");
  // }
  // delay(200);
}

// Returns distance in cm. Returns -1 if timeout / out of range.
float getDistance() {
  // Make sure trigger is low
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 10us trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse width (timeout ~30ms ~ 5m)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL); // timeout is expected by plulseIn to be "unsigned long".

  if (duration == 0) return -1; // timeout/ no object detected

  // Speed of sound: 343 m/s -> 0.0343 cm/us
  // Divide by 2 because it’s there-and-back
  float distanceCm = (duration * 0.0343) / 2.0;

  return distanceCm;
}
