# CTouch Library

Touch controller library for ESP32-2424S012C-I Round LCD and compatible displays.

## Overview

CTouch provides a simple interface for reading touch input from the GT911 (or compatible) touch controller commonly found on round LCD displays. The library uses the Arduino `Wire` library for I2C communication.

## Hardware

- **Display**: ESP32-2424S012C-I Round LCD (240x240 pixels)
- **Touch Controller**: GT911 (I2C address: 0x15)
- **Interface**: I2C (Wire library)

## Wiring

| Display Pin | ESP32 Pin |
|-------------|-----------|
| SDA         | GPIO 4    |
| SCL         | GPIO 5    |

## Installation

### PlatformIO

1. Copy the `CTouch` folder into your project's `lib` directory, OR
2. Add the library to your `platformio.ini` using the `lib_deps` option (if published to registry)

### Arduino IDE

1. Copy the `CTouch` folder into your Arduino libraries folder (`Documents/Arduino/libraries/`)

## Usage

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <CTouch.h>

// Create touch object: SDA pin, SCL pin, I2C address
CTouch touch(4, 5, 0x15);

void setup() {
    Serial.begin(115200);
    
    // Initialize touch controller
    touch.begin();
    
    Serial.println("Touch test ready!");
}

void loop() {
    // Check if screen is being touched
    if (touch.isTouched()) {
        int x = touch.getX();
        int y = touch.getY();
        
        Serial.print("Touch at: ");
        Serial.print(x);
        Serial.print(", ");
        Serial.println(y);
        
        // Wait for release
        while (touch.isTouched()) {
            delay(10);
        }
    }
    
    delay(10);
}
```

## API Reference

### Constructor

```cpp
CTouch(int sda, int scl, uint8_t addr = 0x15)
```

- `sda`: GPIO pin number for I2C SDA
- `scl`: GPIO pin number for I2C SCL  
- `addr`: I2C device address (default 0x15)

### Methods

#### `void begin(uint32_t clockSpeed = 100000)`

Initialize the touch controller.

- `clockSpeed`: I2C clock speed in Hz (default 100kHz)

#### `bool isTouched()`

Check if the screen is currently being touched.

- Returns: `true` if touched, `false` otherwise

#### `int getX()`

Get the X coordinate of the touch.

- Returns: X coordinate (0-239 for 240px wide display)

#### `int getY()`

Get the Y coordinate of the touch.

- Returns: Y coordinate (0-239 for 240px tall display)

#### `bool readTouch(int& x, int& y)`

Convenience method to read touch data in one call.

- `x`: Reference to store X coordinate
- `y`: Reference to store Y coordinate
- Returns: `true` if touched, `false` otherwise

#### `void setAddress(uint8_t addr)`

Set the I2C address.

- `addr`: New I2C address

#### `uint8_t getAddress() const`

Get the current I2C address.

- Returns: Current I2C address

## Example Projects

See the main project in `src/main.cpp` for a complete example that tests all display colors with touch interaction.

## License

MIT License
