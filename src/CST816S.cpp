/**
 * CST816S Touch Controller Driver Implementation
 */

#include <Arduino.h>
#include <CST816S.h>

CST816S::CST816S(uint8_t sda, uint8_t scl, int8_t rst, int8_t intPin)
    : _sda(sda), _scl(scl), _rst(rst), _intPin(intPin)
    , _x(0), _y(0), _gesture(0), _touched(false), _newData(false) 
{
}

bool CST816S::begin() {
    // Initialize I2C
    Wire.begin(_sda, _scl);
    Wire.setClock(100000); // 100kHz
    
    // Reset if pin provided
    if (_rst >= 0) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(50);
    }
    
    // Check device ID
    uint8_t id = readReg(CST816S_REG_ID);
    Serial.print("CST816S ID: 0x");
    Serial.println(id, HEX);
    
    // If we get a response, it's connected
    return (id != 0xFF && id != 0x00);
}

bool CST816S::touched() {
    return _touched;
}

uint16_t CST816S::getX() {
    return _x;
}

uint16_t CST816S::getY() {
    return _y;
}

uint8_t CST816S::getGesture() {
    return _gesture;
}

bool CST816S::hasData() {
    if (_newData) {
        _newData = false;
        return true;
    }
    return false;
}

void CST816S::readTouch() {
    uint8_t buffer[7];
    readRegs(CST816S_REG_MODE, buffer, 7);
    
    Serial.print("Read: ");
    for(int i=0; i<7; i++) {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // Status is at index 1
    uint8_t status = buffer[1];
    Serial.print("Status=0x");
    Serial.print(status, HEX);
    Serial.print(" bit7=");
    Serial.println((status >> 7) & 1);
    
    if (status & 0x80) {
        // Touch detected
        _touched = true;
        _newData = true;
        
        // Get X position (12-bit)
        _x = ((buffer[2] & 0x0F) << 8) | buffer[3];
        
        // Get Y position (12-bit)
        _y = ((buffer[4] & 0x0F) << 8) | buffer[5];
        
        // Get gesture
        _gesture = buffer[0];
        
        Serial.print("TOUCH! X=");
        Serial.print(_x);
        Serial.print(" Y=");
        Serial.println(_y);
    } else {
        _touched = false;
    }
}

void CST816S::writeReg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(CST816S_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t CST816S::readReg(uint8_t reg) {
    Wire.beginTransmission(CST816S_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(CST816S_ADDR, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0xFF;
}

void CST816S::readRegs(uint8_t reg, uint8_t* buffer, uint8_t len) {
    Wire.beginTransmission(CST816S_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(CST816S_ADDR, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
}
