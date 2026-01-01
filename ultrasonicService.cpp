#include "ultrasonicService.h"

static uint8_t g_trig, g_echo;

void registerUltrasonicSensor(uint8_t trigPin, uint8_t echoPin) {
  g_trig = trigPin;
  g_echo = echoPin;

  pinMode(g_trig, OUTPUT);
  pinMode(g_echo, INPUT);
  digitalWrite(g_trig, LOW);
}

float getDistanceInCm() {
  // trigger pulse
  digitalWrite(g_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(g_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(g_trig, LOW);

  // timeout 30ms (~5m)
  unsigned long duration = pulseIn(g_echo, HIGH, 30000UL);
  if (duration == 0) return -1;

  return (duration * 0.0343) / 2.0;
}
