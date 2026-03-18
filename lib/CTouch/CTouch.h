/**
 * @brief CTouch - Touch controller library for ESP32-2424S012C-I Round LCD
 * This library provides an interface to the CTouch touch controller
 * for the ESP32-2424S012C-I round LCD display (typically GT911 or similar).
 * Designed for 240x240 circular LCDs using an unknown controller
 * Author: Ash - A custom driver for LVGL integration
 *
 * @author
 * @version 1.0.0
 * @date 2026
 *
 * Usage:
 *   #include <CTouch.h>
 *
 *   CTouch touch(4, 5, 0x15);  // SDA, SCL, I2C address
 *
 *   void setup() {
 *     touch.begin();
 *   }
 *
 *   void loop() {
 *     if (touch.isTouched()) {
 *       int x = touch.getX();
 *       int y = touch.getY();
 *       // Handle touch
 *     }
 *   }
 */

#ifndef CTOUCH_H
#define CTOUCH_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Default display dimensions
 */
#define CTOUCH_DEFAULT_WIDTH  240
#define CTOUCH_DEFAULT_HEIGHT 240

/**
 * @brief Default calibration values for ESP32-2424S012C-I Round LCD
 * 
 * These values map raw touch controller coordinates to screen coordinates.
 * Modify these for different displays or calibrate per-device.
 */
#define CTOUCH_DEFAULT_RAW_X_MIN      14
#define CTOUCH_DEFAULT_RAW_X_MAX      215
#define CTOUCH_DEFAULT_RAW_Y_MIN      28
#define CTOUCH_DEFAULT_RAW_Y_MAX      225
#define CTOUCH_DEFAULT_SCREEN_X_MIN   0
#define CTOUCH_DEFAULT_SCREEN_X_MAX   239
#define CTOUCH_DEFAULT_SCREEN_Y_MIN   239
#define CTOUCH_DEFAULT_SCREEN_Y_MAX   0

// Constrain range for touch coordinates
#define CTOUCH_SCREEN_MIN             0
#define CTOUCH_SCREEN_MAX             239

/**
 * @brief Rotation modes for touch coordinates
 */
enum class CTouchRotation {
    ROTATION_0,           ///< No rotation
    ROTATION_90,          ///< 90 degrees clockwise
    ROTATION_180,         ///< 180 degrees
    ROTATION_270          ///< 90 degrees counter-clockwise
};

/**
 * @brief CTouch class for touch screen control
 */
class CTouch {
public:
    /**
     * @brief Construct a new CTouch object
     * 
     * @param sda I2C SDA pin number
     * @param scl I2C SCL pin number
     * @param addr I2C device address (default 0x15 for GT911)
     */
    CTouch(int sda, int scl, uint8_t addr = 0x15);
    
    /**
     * @brief Destroy the CTouch object
     */
    ~CTouch();
    
    /**
     * @brief Initialize the touch controller
     * 
     * Initializes the Wire library and configures I2C communication
     * for the touch controller.
     * 
     * @param clockSpeed I2C clock speed in Hz (default 100000)
     */
    void begin(uint32_t clockSpeed = 100000);
    
    /**
     * @brief Check if the screen is currently being touched
     * 
     * Reads the touch status from the controller and returns
     * whether a touch is detected.
     * 
     * @return true if screen is being touched, false otherwise
     */
    bool isTouched();
    
    /**
     * @brief Get the X coordinate of the touch
     * 
     * Must be called after isTouched() returns true.
     * Returns the last read X coordinate.
     * 
     * @return X coordinate (0-240 typically for 240x240 displays)
     */
    int getX();
    
    /**
     * @brief Get the Y coordinate of the touch
     * 
     * Must be called after isTouched() returns true.
     * Returns the last read Y coordinate.
     * 
     * @return Y coordinate (0-240 typically for 240x240 displays)
     */
    int getY();
    
    /**
     * @brief Read touch data and check for touch in one call
     * 
     * This is a convenience method that reads touch data and
     * returns the touch status in a single I2C transaction.
     * 
     * @param x Reference to store X coordinate
     * @param y Reference to store Y coordinate
     * @return true if touched, false otherwise
     */
    bool readTouch(int& x, int& y);
    
    /**
     * @brief Set the I2C address
     * 
     * @param addr New I2C address
     */
    void setAddress(uint8_t addr);
    
    /**
     * @brief Get the current I2C address
     * 
     * @return Current I2C address
     */
    uint8_t getAddress() const;
    
    /**
     * @brief Set the rotation mode for touch coordinates
     * 
     * @param rotation Rotation mode (default ROTATION_270 for 90deg CCW)
     */
    void setRotation(CTouchRotation rotation);
    
    /**
     * @brief Set the display dimensions for coordinate transformation
     * 
     * @param width Display width in pixels
     * @param height Display height in pixels
     */
    void setDimensions(uint16_t width, uint16_t height);
    
    /**
     * @brief Set calibration parameters for mapping raw touch to screen coordinates
     * 
     * The touch controller returns raw X/Y values that need to be mapped to
     * screen coordinates. This method sets the calibration values.
     * 
     * @param rawXmin Minimum raw X value (from touch controller)
     * @param rawXmax Maximum raw X value (from touch controller)
     * @param rawYmin Minimum raw Y value (from touch controller)
     * @param rawYmax Maximum raw Y value (from touch controller)
     * @param screenXmin Minimum screen X coordinate (typically 0)
     * @param screenXmax Maximum screen X coordinate (typically display width-1)
     * @param screenYmin Minimum screen Y coordinate (typically 0 or height-1)
     * @param screenYmax Maximum screen Y coordinate (typically display height-1 or 0)
     */
    void setCalibration(int rawXmin, int rawXmax, int rawYmin, int rawYmax,
                        int screenXmin, int screenXmax, int screenYmin, int screenYmax);
    
    /**
     * @brief Apply default calibration for ESP32-2424S012C-I round display
     * 
     * Sets default calibration values for the round 240x240 display.
     */
    void setDefaultCalibration();

private:
    int _sda;           ///< I2C SDA pin
    int _scl;           ///< I2C SCL pin
    uint8_t _addr;      ///< I2C device address
    int _rawX;          ///< Last raw X coordinate from controller
    int _rawY;          ///< Last raw Y coordinate from controller
    int _touchX;        ///< Calibrated X coordinate
    int _touchY;        ///< Calibrated Y coordinate
    bool _touched;      ///< Last touch status
    CTouchRotation _rotation;  ///< Rotation mode
    uint16_t _width;    ///< Display width
    uint16_t _height;   ///< Display height
    
    // Calibration parameters
    int _rawXmin;       ///< Raw X minimum
    int _rawXmax;       ///< Raw X maximum
    int _rawYmin;       ///< Raw Y minimum
    int _rawYmax;       ///< Raw Y maximum
    int _screenXmin;    ///< Screen X minimum
    int _screenXmax;    ///< Screen X maximum
    int _screenYmin;    ///< Screen Y minimum
    int _screenYmax;    ///< Screen Y maximum
    
    /**
     * @brief Read touch data from controller
     * 
     * Performs I2C communication to read touch status and coordinates.
     * 
     * @return true if data was successfully read, false otherwise
     */
    bool readTouchData();
};

#endif // CTOUCH_H
