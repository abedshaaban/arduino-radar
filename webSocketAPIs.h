#pragma once
#include <Arduino.h>

void setupWebSocketAPI();
void loopWebSocketAPI();
void broadcastDistance(float distanceCm, float angle);

// Callback functions that radar.ino must implement
bool getSystemState();
bool toggleSystemState();
bool getServoRotationEnabled();
bool toggleServoRotation();
void setServoPosition(float angle);
