/**
 * CST816S Touch Controller Driver
 * For ESP32-2424S012C-I Round Display
 * 
 * I2C Address: 0x15
 * 
 * Pin connections (ESP32-C3):
 * - SDA: GPIO 8
 * - SCL: GPIO 9
 * - RST: GPIO 5 (optional)
 * - INT: GPIO 4 (interrupt - optional)
 */

#ifndef CST816S_H
#define CST816S_H

#include <Arduino.h>
#include <Wire.h>

// CST816S I2C Address
#define CST816S_ADDR 0x15

// Register addresses
#define CST816S_REG_MODE      0x00
#define CST816S_REG_X_POS_H   0x03
#define CST816S_REG_X_POS_L   0x04
#define CST816S_REG_Y_POS_H   0x05
#define CST816S_REG_Y_POS_L   0x06
#define CST816S_REG Gesture   0x01
#define CST816S_REG_STATUS    0x02
#define CST816S_REG_ID        0xA8
#define CST816S_REG_THGROUP   0x80
#define CST816S_REG_THPEAK    0x81

// Gesture types
#define CST816S_GESTURE_NONE      0x00
#define CST816S_GESTURE_CLICK     0x1C
#define CST816S_GESTURE_SWIPE_UP  0x28
#define CST816S_GESTURE_SWIPE_DOWN 0x29
#define CST816S_GESTURE_SWIPE_LEFT 0x5A
#define CST816S_GESTURE_SWIPE_RIGHT 0x5B

class CST816S {
public:
    /**
     * @brief Construct a new CST816S object
     * @param sda SDA pin
     * @param scl SCL pin
     * @param rst Reset pin (-1 if not used)
     * @param intPin Interrupt pin (-1 if not used)
     */
    CST816S(uint8_t sda = 8, uint8_t scl = 9, int8_t rst = -1, int8_t intPin = -1);
    
    /**
     * @brief Initialize the touch controller
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Check if touch is detected
     * @return true if touched
     */
    bool touched();
    
    /**
     * @brief Get touch X position
     * @return X coordinate (0-239 for 240 wide)
     */
    uint16_t getX();
    
    /**
     * @brief Get touch Y position  
     * @return Y coordinate (0-239 for 240 high)
     */
    uint16_t getY();
    
    /**
     * @brief Get gesture type
     * @return Gesture code
     */
    uint8_t getGesture();
    
    /**
     * @brief Read touch data
     * Should be called in loop
     */
    void readTouch();
    
    /**
     * @brief Check if we have new data
     */
    bool hasData();
    
private:
    uint8_t _sda, _scl;
    int8_t _rst, _intPin;
    uint16_t _x, _y;
    uint8_t _gesture;
    bool _touched;
    bool _newData;
    
    /**
     * @brief Write a register
     */
    void writeReg(uint8_t reg, uint8_t value);
    
    /**
     * @brief Read a register
     */
    uint8_t readReg(uint8_t reg);
    
    /**
     * @brief Read multiple registers
     */
    void readRegs(uint8_t reg, uint8_t* buffer, uint8_t len);
};

#endif // CST816S_H
