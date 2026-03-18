/**
 * @brief CTouch - Touch controller library implementation
 * This library provides an interface to the CTouch touch controller
 * for the ESP32-2424S012C-I round LCD display (typically GT911 or similar).
 * Designed for 240x240 circular LCDs using an unknown controller
 * Author: Ash - A custom driver for LVGL integration
 *
 * @see CTouch.h
 */

#include <CTouch.h>

CTouch::CTouch(int sda, int scl, uint8_t addr)
    : _sda(sda)
    , _scl(scl)
    , _addr(addr)
    , _rawX(0)
    , _rawY(0)
    , _touchX(0)
    , _touchY(0)
    , _touched(false)
    , _rotation(CTouchRotation::ROTATION_270)
    , _width(CTOUCH_DEFAULT_WIDTH)
    , _height(CTOUCH_DEFAULT_HEIGHT)
    // Default calibration values
    , _rawXmin(CTOUCH_DEFAULT_RAW_X_MIN)
    , _rawXmax(CTOUCH_DEFAULT_RAW_X_MAX)
    , _rawYmin(CTOUCH_DEFAULT_RAW_Y_MIN)
    , _rawYmax(CTOUCH_DEFAULT_RAW_Y_MAX)
    , _screenXmin(CTOUCH_DEFAULT_SCREEN_X_MIN)
    , _screenXmax(CTOUCH_DEFAULT_SCREEN_X_MAX)
    , _screenYmin(CTOUCH_DEFAULT_SCREEN_Y_MIN)
    , _screenYmax(CTOUCH_DEFAULT_SCREEN_Y_MAX)
{
}

CTouch::~CTouch() {
}

void CTouch::begin(uint32_t clockSpeed) {
    // Initialize I2C with specified pins
    Wire.begin(_sda, _scl);
    Wire.setClock(clockSpeed);
}

bool CTouch::isTouched() {
    // Read touch data from controller
    if (readTouchData()) {
        return _touched;
    }
    return false;
}

int CTouch::getX() {
    // Apply calibration: screenX = map(rawY, rawYmin, rawYmax, screenXmin, screenXmax)
    // The touch controller swaps X and Y, so rawY maps to screenX
    int screenX = map(_rawY, _rawYmin, _rawYmax, _screenXmin, _screenXmax);
    screenX = constrain(screenX, CTOUCH_SCREEN_MIN, CTOUCH_SCREEN_MAX);
    return screenX;
}

int CTouch::getY() {
    // Apply calibration: screenY = map(rawX, rawXmin, rawXmax, screenYmin, screenYmax)
    // The touch controller swaps X and Y, so rawX maps to screenY
    int screenY = map(_rawX, _rawXmin, _rawXmax, _screenYmin, _screenYmax);
    screenY = constrain(screenY, CTOUCH_SCREEN_MIN, CTOUCH_SCREEN_MAX);
    return screenY;
}

bool CTouch::readTouch(int& x, int& y) {
    if (readTouchData()) {
        x = getX();
        y = getY();
        
        // Apply rotation transformation
        uint16_t w = _width;
        uint16_t h = _height;
        
        switch (_rotation) {
            case CTouchRotation::ROTATION_0:
                {
                    // For display rotation 0, apply the base transformation
                    // that aligns touch coordinates with the display
                    int tmp = x;
                    x = w - y;
                    y = tmp;
                }
                break;
            case CTouchRotation::ROTATION_90:
                {
                    // 90 degrees clockwise: newX = oldY, newY = w - oldX
                    int tmp = x;
                    x = y;
                    y = w - tmp;
                }
                break;
            case CTouchRotation::ROTATION_180:
                x = w - x;
                y = h - y;
                break;
            case CTouchRotation::ROTATION_270:
                {
                    // 270 degrees clockwise (90 CCW): newX = h - oldY, newY = oldX
                    int tmp = x;
                    x = h - y;
                    y = tmp;
                }
                break;
            default:
                break;
        }
        
        return _touched;
    }
    x = 0;
    y = 0;
    return false;
}

void CTouch::setAddress(uint8_t addr) {
    _addr = addr;
}

uint8_t CTouch::getAddress() const {
    return _addr;
}

void CTouch::setRotation(CTouchRotation rotation) {
    _rotation = rotation;
}

void CTouch::setDimensions(uint16_t width, uint16_t height) {
    _width = width;
    _height = height;
}

void CTouch::setCalibration(int rawXmin, int rawXmax, int rawYmin, int rawYmax,
                            int screenXmin, int screenXmax, int screenYmin, int screenYmax) {
    _rawXmin = rawXmin;
    _rawXmax = rawXmax;
    _rawYmin = rawYmin;
    _rawYmax = rawYmax;
    _screenXmin = screenXmin;
    _screenXmax = screenXmax;
    _screenYmin = screenYmin;
    _screenYmax = screenYmax;
}

void CTouch::setDefaultCalibration() {
    // Default calibration values
    _rawXmin = CTOUCH_DEFAULT_RAW_X_MIN;
    _rawXmax = CTOUCH_DEFAULT_RAW_X_MAX;
    _rawYmin = CTOUCH_DEFAULT_RAW_Y_MIN;
    _rawYmax = CTOUCH_DEFAULT_RAW_Y_MAX;
    _screenXmin = CTOUCH_DEFAULT_SCREEN_X_MIN;
    _screenXmax = CTOUCH_DEFAULT_SCREEN_X_MAX;
    _screenYmin = CTOUCH_DEFAULT_SCREEN_Y_MIN;
    _screenYmax = CTOUCH_DEFAULT_SCREEN_Y_MAX;
}

bool CTouch::readTouchData() {
    // Request touch status from controller
    Wire.beginTransmission(_addr);
    Wire.write(0x01);  // Command to get touch status
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        // I2C error occurred
        _touched = false;
        return false;
    }
    
    // Request 6 bytes of data from touch controller
    // Format: [0]header [1]status [2]? [3]X [4]? [5]Y
    Wire.requestFrom(_addr, (uint8_t)6);
    
    if (Wire.available() >= 6) {
        Wire.read();        // Discard first byte (header)
        byte status = Wire.read();  // Status byte
        Wire.read();        // Discard
        byte xByte = Wire.read();   // X position (raw)
        Wire.read();        // Discard
        byte yByte = Wire.read();   // Y position (raw)
        
        // Check if touched (bit 0 of status)
        _touched = (status & 0x01) != 0;
        
        if (_touched) {
            _rawX = xByte;
            _rawY = yByte;
        }
        
        return true;
    }
    
    _touched = false;
    return false;
}
