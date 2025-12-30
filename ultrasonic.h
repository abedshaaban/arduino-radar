#pragma once
#include <Arduino.h>

void registerUltrasonicSensor(uint8_t trigPin, uint8_t echoPin);
float getDistanceInCm(); // returns -1 on timeout or when no object is detected
