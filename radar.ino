#include "ultrasonic.h"

const int TRIG_PIN = 18;
const int ECHO_PIN = 19;

void setup() {
  Serial.begin(115200);

  registerUltrasonicSensor(TRIG_PIN, ECHO_PIN);
}

void loop() {
  // read the distance
  static unsigned long last = 0;
  if (millis() - last >= 200) {
    last = millis();
    float d = getDistanceInCm();
    Serial.println(String("Distance: ") + String(d) + " cm");
  }
}
