/**
 * GC9A01 Direct Driver for ESP32-C3 Round Display
 * Designed for 240x240 circular LCDs using the GC9A01 controller
 * Author: Ash - A custom driver for LVGL integration
 *
 * Pin Configuration:
 * - SCLK: GPIO 6
 * - MOSI: GPIO 7
 * - CS:   GPIO 10
 * - DC:   GPIO 2
 * - RST:  GPIO 0
 * - BL:   GPIO 3
 */

#ifndef GC9A01_H
#define GC9A01_H

#include <Arduino.h>
#include <SPI.h>

// Display dimensions
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

class GC9A01 {
public:
    /**
     * @brief Construct a new GC9A01 object
     * @param clk SPI Clock pin
     * @param mosi SPI MOSI pin
     * @param cs Chip Select pin
     * @param dc Data/Command pin
     * @param rst Reset pin
     * @param bl Backlight pin
     */
    GC9A01(uint8_t clk = 6, uint8_t mosi = 7, uint8_t cs = 10, 
           uint8_t dc = 2, uint8_t rst = 0, uint8_t bl = 3);
    
    /**
     * @brief Initialize the display
     */
    void init();
    
    /**
     * @brief Fill the entire screen with a color
     * @param color 16-bit RGB565 color
     */
    void fillScreen(uint16_t color);
    
    /**
     * @brief Draw a single pixel
     * @param x X coordinate
     * @param y Y coordinate
     * @param color 16-bit RGB565 color
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    
    /**
     * @brief Draw a filled circle
     * @param x0 Center X
     * @param y0 Center Y
     * @param r Radius
     * @param color 16-bit RGB565 color
     */
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    
    /**
     * @brief Draw a circle outline
     * @param x0 Center X
     * @param y0 Center Y
     * @param r Radius
     * @param color 16-bit RGB565 color
     */
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    
    /**
     * @brief Draw a line
     * @param x0 Start X
     * @param y0 Start Y
     * @param x1 End X
     * @param y1 End Y
     * @param color 16-bit RGB565 color
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    
    /**
     * @brief Draw a horizontal line
     * @param x Start X
     * @param y Y coordinate
     * @param w Width
     * @param color 16-bit RGB565 color
     */
    void drawHorizontalLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
    
    /**
     * @brief Draw a vertical line
     * @param x X coordinate
     * @param y Start Y
     * @param h Height
     * @param color 16-bit RGB565 color
     */
    void drawVerticalLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
    
    /**
     * @brief Draw a rectangle (outline)
     * @param x Top-left X
     * @param y Top-left Y
     * @param w Width
     * @param h Height
     * @param color 16-bit RGB565 color
     */
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    
    /**
     * @brief Fill a rectangle
     * @param x Top-left X
     * @param y Top-left Y
     * @param w Width
     * @param h Height
     * @param color 16-bit RGB565 color
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    
    /**
     * @brief Set the text color
     * @param color Foreground color
     * @param bg Background color
     */
    void setTextColor(uint16_t color, uint16_t bg = 0x0000);
    
    /**
     * @brief Set text size
     * @param size Size multiplier (1-8)
     */
    void setTextSize(uint8_t size);
    
    /**
     * @brief Draw a text string
     * @param x X position
     * @param y Y position
     * @param text String to draw
     */
    void drawString(const char* text, uint16_t x, uint16_t y);
    
    /**
     * @brief Draw a number
     * @param x X position
     * @param y Y position
     * @param num Number to draw
     */
    void drawNumber(int32_t num, uint16_t x, uint16_t y);
    
    /**
     * @brief Set display rotation
     * @param rotation 0-3
     */
    void setRotation(uint8_t rotation);
    
    /**
     * @brief Get display width
     */
    uint16_t width() { return _width; }
    
    /**
     * @brief Get display height
     */
    uint16_t height() { return _height; }
    
    /**
     * @brief Turn on backlight
     * @param brightness 0-255
     */
    void setBacklight(uint8_t brightness);
    
    // Predefined colors (RGB565)
    static const uint16_t TFT_BLACK   = 0x0000;
    static const uint16_t TFT_WHITE   = 0xFFFF;
    static const uint16_t TFT_RED     = 0xF800;
    static const uint16_t TFT_GREEN   = 0x07E0;
    static const uint16_t TFT_BLUE    = 0x001F;
    static const uint16_t TFT_YELLOW   = 0xFFE0;
    static const uint16_t TFT_CYAN    = 0x07FF;
    static const uint16_t TFT_MAGENTA = 0xF81F;
    static const uint16_t TFT_ORANGE  = 0xFC00;
    static const uint16_t TFT_PINK    = 0xF81F;
    static const uint16_t TFT_PURPLE  = 0x8010;
    static const uint16_t TFT_NAVY    = 0x0010;
    static const uint16_t TFT_DARKGREEN = 0x03E0;
    static const uint16_t TFT_DARKCYAN = 0x03EF;
    static const uint16_t TFT_MAROON  = 0x7800;
    static const uint16_t TFT_OLIVE   = 0x7BE0;
    static const uint16_t TFT_LIGHTGREY = 0xC618;
    static const uint16_t TFT_DARKGREY = 0x4208;
    static const uint16_t TFT_GREY    = 0x8410;
 
    // Low-level methods for LVGL driver (public for external access)
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void setAddr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void pushColor(uint16_t color);
    void pushColors(uint16_t* colors, uint32_t len);
    void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

private:
    uint8_t _clk, _mosi, _cs, _dc, _rst, _bl;
    uint16_t _width, _height;
    uint16_t _textColor, _textBgColor;
    uint8_t _textSize;
    uint8_t _rotation;
};

#endif // GC9A01_H
