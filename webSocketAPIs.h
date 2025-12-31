#pragma once
#include <Arduino.h>

void setupWebSocketAPI();
void loopWebSocketAPI();
void broadcastDistance(float distanceCm);

// Callback functions that radar.ino must implement
bool getSystemState();
bool toggleSystemState();
