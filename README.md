# Round Klipper Display

A custom round LCD touch panel for Klipper 3D printer firmware, built with ESP32-C3 and LVGL.

## Overview

This project provides a graphical touch interface for Klipper-based 3D printers using a round 240x240 LCD display. It connects to Moonraker (Klipper's API server) via WebSocket to display real-time printer status and temperatures.

![Round Display](https://img.shields.io/badge/Display-240x240_round-blue)
![ESP32-C3](https://img.shields.io/badge/ESP32-C3?label=Device)
![Klipper](https://img.shields.io/badge/Klipper-Compatible-orange)

## Hardware

- **Board**: ESP32-C3 (ESP32-2424S012C-I Round LCD Module)
- **Display**: GC9A01 Round LCD (240x240 pixels)
- **Touch Controller**: CST816S / GT911 (I2C)
- **Resolution**: 240 x 240 pixels (circular)

### Pin Configuration

| Function | GPIO Pin |
|----------|----------|
| LCD SCK  | GPIO 6   |
| LCD MOSI | GPIO 7   |
| LCD CS   | GPIO 10  |
| LCD DC   | GPIO 2   |
| LCD RST  | GPIO 1   |
| Touch SDA| GPIO 4   |
| Touch SCL| GPIO 5   |

## Features

- **Real-time Temperature Display**: Shows hotend and bed temperatures
- **Print Status**: Displays current printer state (printing, idle, ready, etc.)
- **Touch Controls**: Three interactive buttons for:
  - Resume Print
  - Load Filament
  - Unload Filament
- **WebSocket Connection**: Direct communication with Moonraker API
- **Custom UI**: Round-optimized interface using LVGL

## Prerequisites

### Klipper Setup

1. Install [Klipper](https://www.klipper3d.org/) on your 3D printer
2. Install [Moonraker](https://github.com/Arksine/moonraker) (usually included with Klipper)
3. Get your Moonraker API key:
   - Navigate to `http://klipper.local/access/api_key`
   - Note down your API key

### Software Requirements

- [PlatformIO](https://platformio.org/) (recommended)
- Arduino framework
- LVGL v9.1.0+
- WebSockets library

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/Round-Klipper-Display.git
cd Round-Klipper-Display
```

### 2. Configure WiFi & Moonraker

Edit [`include/round_klipper_conf.h`](include/round_klipper_conf.h) and update:

```cpp
// WiFi credentials
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define HOST_NAME "ENDER3_PANEL"

// Moonraker connection
#define MOONRAKER_HOST "klipper.local"  // Your Klipper hostname/IP
#define MOONRAKER_PORT 7125              // Default Moonraker port
#define MOONRAKER_API_KEY "your_api_key" // Your Moonraker API key
```

### 3. Configure Button Macros

Update the button actions in [`include/round_klipper_conf.h`](include/round_klipper_conf.h) to match your Klipper macros:

```cpp
#define RESUME_BTN_MACRO "RESUME"           // Macro to resume print
#define BOTTOM_LEFT_BTN_MACRO "LOAD_FILAMENT"  // Macro to load filament
#define BOTTOM_RIGHT_BTN_MACRO "UNLOAD_FILAMENT" // Macro to unload filament
```

### 4. Build and Upload

Using PlatformIO:

```bash
# Build the project
pio run

# Upload to ESP32-C3
pio run --target upload

# Monitor serial output
pio device monitor
```

## Project Structure

```
Round-Klipper-Display/
├── include/
│   ├── round_klipper_conf.h    # Configuration (WiFi, Moonraker, buttons)
│   ├── GC9A01.h                 # Display driver header
│   ├── CST816S.h               # Touch controller header
│   ├── UI.h                    # UI function declarations
│   └── lv_conf.h               # LVGL configuration
├── src/
│   ├── main.cpp                # Main program
│   ├── GC9A01.cpp              # Display driver implementation
│   ├── CST816S.cpp             # Touch controller implementation
│   ├── UI.cpp                  # UI implementation
│   └── lv_drivers/
│       └── display/
│           └── GC9A01_lvgl.cpp  # LVGL display driver
├── lib/
│   └── CTouch/                 # Touch library (GT911 compatible)
├── platformio.ini              # PlatformIO configuration
└── README.md                   # This file
```

## Customization

### Changing Display Pins

If you're using a different round display module, modify the pin definitions in [`include/round_klipper_conf.h`](include/round_klipper_conf.h):

```cpp
#define LCD_CS   10
#define LCD_DC   2
#define LCD_RST  1
#define LCD_SCK  6
#define LCD_MOSI 7

#define TOUCH_SDA 4
#define TOUCH_SCL 5
#define TOUCH_I2C_ADDR 0x15
```

### Adjusting Touch Rotation

In [`src/main.cpp`](src/main.cpp), change the touch rotation:

```cpp
touch.setRotation(CTouchRotation::ROTATION_0);  // 0, 90, 180, or 270
```

### Modifying the UI

The UI is built with LVGL in [`src/UI.cpp`](src/UI.cpp). You can customize:
- Colors and fonts
- Button layouts
- Animation effects
- Additional information display

## Troubleshooting

### Display Not Working
- Check SPI connections (SCK, MOSI, CS, DC)
- Verify display controller is GC9A01
- Check voltage (should be 3.3V)

### Touch Not Responding
- Verify I2C connections (SDA, SCL)
- Check touch controller I2C address (default 0x15)
- Ensure correct pull-up resistors on I2C lines

### Cannot Connect to Moonraker
- Verify WiFi credentials
- Check Moonraker is running on your Klipper host
- Ensure API key is correct
- Verify network connectivity (ping klipper.local)

### Temperature Not Updating
- Check Moonraker WebSocket connection
- Verify API key permissions
- Ensure Klipper is running and printer is connected

## Dependencies

- [LVGL](https://lvgl.io/) v9.1.0+ - Graphics library
- [WebSockets](https://github.com/Links2004/ArduinoWebSockets) - WebSocket client for Moonraker
- [Arduino](https://www.arduino.cc/) - ESP32 framework

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [LVGL](https://lvgl.io/) for the excellent graphics library
- [Klipper](https://www.klipper3d.org/) community
- [Moonraker](https://github.com/Arksine/moonraker) for the Klipper API

## Support

If you encounter issues:
1. Check the serial output at 115200 baud
2. Verify all connections
3. Ensure WiFi network is 2.4GHz (ESP32-C3 doesn't support 5GHz)
4. Check Klipper/Moonraker logs
