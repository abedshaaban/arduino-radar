# ESP32 Radar System

A real-time distance monitoring system built with ESP32 that uses an ultrasonic sensor and servo motor to create a scanning radar. The system provides a web-based interface accessible via WiFi for monitoring distance measurements in real-time.

## Features

- **Real-time Distance Monitoring**: Continuous distance measurements using HC-SR04 ultrasonic sensor
- **Servo Scanning**: Automatic 180-degree servo rotation for scanning
- **Web Interface**: Beautiful, responsive web UI accessible via WiFi
- **WebSocket Communication**: Real-time bidirectional communication between ESP32 and web client
- **System Control**: Start/stop the radar system remotely via web interface
- **WiFi Access Point**: Creates its own WiFi network for easy connection

## Hardware Requirements

- ESP32 development board
- HC-SR04 ultrasonic distance sensor
- Servo motor (SG90 or similar)
- Jumper wires
- Breadboard (optional)
- Power supply (USB or external)

## Pin Connections

| Component              | ESP32 Pin |
| ---------------------- | --------- |
| Ultrasonic Sensor TRIG | GPIO 18   |
| Ultrasonic Sensor ECHO | GPIO 19   |
| Servo Motor Signal     | GPIO 5    |

## Software Requirements

- [Arduino CLI](https://arduino.github.io/arduino-cli/) installed and configured
- ESP32 board support package for Arduino CLI
- Required Arduino libraries:
  - `ESP32Servo`
  - `WebServer` (included with ESP32)
  - `WebSocketsServer` (included with ESP32)
  - `ArduinoJson`

## Installation

### 1. Install ESP32 Board Support

```bash
arduino-cli core install esp32:esp32
```

### 2. Install Required Libraries

```bash
arduino-cli lib install ESP32Servo ArduinoJson
```

### 3. Configure WiFi Credentials

Edit `radar.ino` and update the WiFi credentials:

```cpp
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";
```

## Usage

### 1. Compile the Project

```bash
arduino-cli compile --clean --fqbn esp32:esp32:esp32 .
```

### 2. Upload to ESP32

Connect your ESP32 via USB and upload the code:

```bash
arduino-cli upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 .
```

**Note**: Replace `/dev/cu.usbserial-0001` with your actual serial port. On Windows, it will be something like `COM3`. On Linux, it might be `/dev/ttyUSB0`.

### 3. Monitor Serial Output

To view debug logs and system status:

```bash
arduino-cli monitor -p /dev/cu.usbserial-0001 -c baudrate=115200
```

### 4. Connect to the Radar System

1. Power on the ESP32
2. Connect to the WiFi network (SSID and password as configured in `radar.ino`)
3. Open a web browser and navigate to `http://192.168.4.1` (default ESP32 AP IP)
4. Click "Start System" to begin scanning
5. View real-time distance measurements on the web interface

## Project Structure

```
radar/
├── radar.ino              # Main Arduino sketch
├── ultrasonicService.h    # Ultrasonic sensor interface
├── ultrasonicService.cpp  # Ultrasonic sensor implementation
├── wifiService.h          # WiFi access point interface
├── wifiService.cpp        # WiFi access point implementation
├── webSocketAPIs.h        # WebSocket API interface
├── webSocketAPIs.cpp      # WebSocket API implementation
├── htmlPage.h             # Embedded HTML web page
├── index.html             # Source HTML file (for reference)
└── README.md              # This file
```

## How It Works

1. **Initialization**: On startup, the ESP32 creates a WiFi access point and starts HTTP and WebSocket servers
2. **Web Interface**: Users connect to the WiFi network and access the web interface
3. **System Control**: The web interface communicates with ESP32 via WebSocket to start/stop the system
4. **Scanning**: When active, the servo rotates continuously from 0° to 180° and back
5. **Distance Measurement**: Every 300ms, the ultrasonic sensor measures distance
6. **Real-time Updates**: Distance readings are broadcast to all connected web clients via WebSocket

## License

This project is open source and available for personal and educational use.
