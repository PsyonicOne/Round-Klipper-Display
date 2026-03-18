/**
 * GC9A01 Direct Driver Implementation for ESP32-C3 Round Display
 * Designed for 240x240 circular LCDs using the GC9A01 controller
 * Author: Ash as a custom driver for LVGL integration
 **/

#include <GC9A01.h>

// Font data for simple 5x7 font
extern const uint8_t font5x7[][5] PROGMEM;

GC9A01::GC9A01(uint8_t clk, uint8_t mosi, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl)
    : _clk(clk), _mosi(mosi), _cs(cs), _dc(dc), _rst(rst), _bl(bl),
      _width(TFT_WIDTH), _height(TFT_HEIGHT),
      _textColor(0xFFFF), _textBgColor(0x0000), _textSize(1), _rotation(0) {
}

void GC9A01::init() {
    // Set pin modes
    pinMode(_cs, OUTPUT);
    pinMode(_dc, OUTPUT);
    pinMode(_rst, OUTPUT);
    pinMode(_bl, OUTPUT);
    
    // Initialize SPI with explicit pins for ESP32-C3
    SPI.begin(_clk, -1, _mosi, _cs);
    SPI.setFrequency(20000000);  // 20MHz for faster rendering
    
    // Hardware reset
    digitalWrite(_rst, LOW);
    delay(50);
    digitalWrite(_rst, HIGH);
    delay(50);
    
    // Full GC9A01 initialization sequence
    writeCommand(0xEF);
    writeCommand(0xEB);
    writeData(0x14);
    
    writeCommand(0xFE);
    writeCommand(0xEF);
    
    writeCommand(0xEB);
    writeData(0x14);
    
    writeCommand(0x84);
    writeData(0x40);
    
    writeCommand(0x85);
    writeData(0xFF);
    
    writeCommand(0x86);
    writeData(0xFF);
    
    writeCommand(0x87);
    writeData(0xFF);
    
    writeCommand(0x88);
    writeData(0x0A);
    
    writeCommand(0x89);
    writeData(0x21);
    
    writeCommand(0x8A);
    writeData(0x00);
    
    writeCommand(0x8B);
    writeData(0x80);
    
    writeCommand(0x8C);
    writeData(0x01);
    
    writeCommand(0x8D);
    writeData(0x01);
    
    writeCommand(0x8E);
    writeData(0xFF);
    
    writeCommand(0x8F);
    writeData(0xFF);
    
    writeCommand(0xB6);
    writeData(0x00);
    writeData(0x20);
    
    writeCommand(0x3A);
    writeData(0x05);
    
    writeCommand(0x90);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);
    
    writeCommand(0xBD);
    writeData(0x06);
    
    writeCommand(0xBC);
    writeData(0x00);
    
    writeCommand(0xFF);
    writeData(0x60);
    writeData(0x01);
    writeData(0x04);
    
    writeCommand(0xC3);
    writeData(0x13);
    writeCommand(0xC4);
    writeData(0x13);
    
    writeCommand(0xC9);
    writeData(0x22);
    
    writeCommand(0xBE);
    writeData(0x11);
    
    writeCommand(0xE1);
    writeData(0x10);
    writeData(0x0E);
    
    writeCommand(0xDF);
    writeData(0x21);
    writeData(0x0C);
    writeData(0x02);
    
    writeCommand(0xF0);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);
    
    writeCommand(0xF1);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);
    
    writeCommand(0xF2);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);
    
    writeCommand(0xF3);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);
    
    writeCommand(0xED);
    writeData(0x1B);
    writeData(0x0B);
    
    writeCommand(0xAE);
    writeData(0x77);
    
    writeCommand(0xCD);
    writeData(0x63);
    
    writeCommand(0x70);
    writeData(0x07);
    writeData(0x07);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x0F);
    writeData(0x09);
    writeData(0x07);
    writeData(0x08);
    writeData(0x03);
    
    writeCommand(0xE8);
    writeData(0x34);
    
    writeCommand(0x62);
    writeData(0x18);
    writeData(0x0D);
    writeData(0x71);
    writeData(0xED);
    writeData(0x70);
    writeData(0x70);
    writeData(0x18);
    writeData(0x0F);
    writeData(0x71);
    writeData(0xEF);
    writeData(0x70);
    writeData(0x70);
    
    writeCommand(0x63);
    writeData(0x18);
    writeData(0x11);
    writeData(0x71);
    writeData(0xF1);
    writeData(0x70);
    writeData(0x70);
    writeData(0x18);
    writeData(0x13);
    writeData(0x71);
    writeData(0xF3);
    writeData(0x70);
    writeData(0x70);
    
    writeCommand(0x64);
    writeData(0x28);
    writeData(0x29);
    writeData(0xF1);
    writeData(0x01);
    writeData(0xF1);
    writeData(0x00);
    writeData(0x07);
    
    writeCommand(0x66);
    writeData(0x3C);
    writeData(0x00);
    writeData(0xCD);
    writeData(0x67);
    writeData(0x45);
    writeData(0x45);
    writeData(0x10);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    
    writeCommand(0x67);
    writeData(0x00);
    writeData(0x3C);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    writeData(0x01);
    writeData(0x54);
    writeData(0x10);
    writeData(0x32);
    writeData(0x98);
    
    writeCommand(0x74);
    writeData(0x10);
    writeData(0x85);
    writeData(0x80);
    writeData(0x00);
    writeData(0x00);
    writeData(0x4E);
    writeData(0x00);
    
    writeCommand(0x98);
    writeData(0x3e);
    writeData(0x07);
    
    writeCommand(0x35);
    writeCommand(0x21);
    
    writeCommand(0x11);
    delay(120);
    writeCommand(0x29);
    delay(20);
    
    // Turn on backlight
    setBacklight(255);
}

void GC9A01::writeCommand(uint8_t cmd) {
    digitalWrite(_dc, LOW);
    digitalWrite(_cs, LOW);
    SPI.transfer(cmd);
    digitalWrite(_cs, HIGH);
}

void GC9A01::writeData(uint8_t data) {
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(data);
    digitalWrite(_cs, HIGH);
}

void GC9A01::setAddr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    writeCommand(0x2A);
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(x1 >> 8);
    SPI.transfer(x1 & 0xFF);
    SPI.transfer(x2 >> 8);
    SPI.transfer(x2 & 0xFF);
    digitalWrite(_cs, HIGH);
    
    writeCommand(0x2B);
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(y1 >> 8);
    SPI.transfer(y1 & 0xFF);
    SPI.transfer(y2 >> 8);
    SPI.transfer(y2 & 0xFF);
    digitalWrite(_cs, HIGH);
}

void GC9A01::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    // Set column address
    writeCommand(0x2A);
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(x1 >> 8);
    SPI.transfer(x1 & 0xFF);
    SPI.transfer(x2 >> 8);
    SPI.transfer(x2 & 0xFF);
    digitalWrite(_cs, HIGH);
    
    // Set row address
    writeCommand(0x2B);
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(y1 >> 8);
    SPI.transfer(y1 & 0xFF);
    SPI.transfer(y2 >> 8);
    SPI.transfer(y2 & 0xFF);
    digitalWrite(_cs, HIGH);
    
    // Send memory write command
    writeCommand(0x2C);
}

void GC9A01::pushColor(uint16_t color) {
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    // Send high byte first
    SPI.transfer(color >> 8);
    SPI.transfer(color & 0xFF);
    digitalWrite(_cs, HIGH);
}

void GC9A01::pushColors(uint16_t* colors, uint32_t len) {
    // Use bulk transfer with pre-allocated static buffer
    // Partial render: 240 x 80 pixels x 2 bytes = 38400 bytes
    static uint8_t buffer[40000];  // Pre-allocated, enough for partial render
    
    uint32_t neededSize = len * 2;
    
    if (neededSize > sizeof(buffer)) {
        // Buffer too small, use fallback
        Serial.print("pushColors: buffer too small (");
        Serial.print(neededSize);
        Serial.println(" bytes), using fallback");
        digitalWrite(_dc, HIGH);
        digitalWrite(_cs, LOW);
        for (uint32_t i = 0; i < len; i++) {
            SPI.transfer(colors[i] >> 8);
            SPI.transfer(colors[i] & 0xFF);
        }
        digitalWrite(_cs, HIGH);
        return;
    }
    
    // Convert from little-endian to big-endian
    for (uint32_t i = 0; i < len; i++) {
        buffer[i * 2] = colors[i] >> 8;
        buffer[i * 2 + 1] = colors[i] & 0xFF;
    }
    
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.writeBytes((const uint8_t*)buffer, neededSize);
    digitalWrite(_cs, HIGH);
}

void GC9A01::fillScreen(uint16_t color) {
    setAddr(0, 0, _width - 1, _height - 1);
    
    writeCommand(0x2C);
    
    uint32_t count = (uint32_t)_width * _height;
    
    // For full screen, use byte-by-byte (too big for static buffer on ESP32-C3)
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    for (uint32_t i = 0; i < count; i++) {
        SPI.transfer(hi);
        SPI.transfer(lo);
    }
    digitalWrite(_cs, HIGH);
}

void GC9A01::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= _width || y >= _height) return;
    
    setAddr(x, y, x, y);
    writeCommand(0x2C);
    pushColor(color);
}

void GC9A01::drawHorizontalLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    if (y >= _height) return;
    if (x + w > _width) w = _width - x;
    
    setAddr(x, y, x + w - 1, y);
    writeCommand(0x2C);
    
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    for (uint16_t i = 0; i < w; i++) {
        SPI.transfer(hi);
        SPI.transfer(lo);
    }
    
    digitalWrite(_cs, HIGH);
}

void GC9A01::drawVerticalLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    if (x >= _width) return;
    if (y + h > _height) h = _height - y;
    
    setAddr(x, y, x, y + h - 1);
    writeCommand(0x2C);
    
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    for (uint16_t i = 0; i < h; i++) {
        SPI.transfer(hi);
        SPI.transfer(lo);
    }
    
    digitalWrite(_cs, HIGH);
}

void GC9A01::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    // Bresenham's line algorithm
    int16_t dx = abs(x1 - x0);
    int16_t dy = -abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    
    while (1) {
        if (x0 >= 0 && x0 < _width && y0 >= 0 && y0 < _height) {
            drawPixel(x0, y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void GC9A01::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x + w > _width) w = _width - x;
    if (y + h > _height) h = _height - y;
    
    setAddr(x, y, x + w - 1, y + h - 1);
    writeCommand(0x2C);
    
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        SPI.transfer(hi);
        SPI.transfer(lo);
    }
    
    digitalWrite(_cs, HIGH);
}

void GC9A01::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHorizontalLine(x, y, w, color);
    drawHorizontalLine(x, y + h - 1, w, color);
    drawVerticalLine(x, y, h, color);
    drawVerticalLine(x + w - 1, y, h, color);
}

void GC9A01::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (r <= 0) return;
    
    // Draw filled circle using horizontal scanlines
    for (int16_t dy = -r; dy <= r; dy++) {
        // Calculate horizontal span at this y offset
        int16_t dx = (int16_t)sqrt((int32_t)r * r - (int32_t)dy * dy);
        // Draw horizontal line from left edge to right edge
        drawHorizontalLine(x0 - dx, y0 + dy, 2 * dx + 1, color);
    }
}

void GC9A01::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void GC9A01::setTextColor(uint16_t color, uint16_t bg) {
    _textColor = color;
    _textBgColor = bg;
}

void GC9A01::setTextSize(uint8_t size) {
    _textSize = size > 0 ? size : 1;
}

void GC9A01::drawString(const char* text, uint16_t x, uint16_t y) {
    uint16_t cursorX = x;
    uint16_t cursorY = y;
    uint8_t w = 5 * _textSize;
    uint8_t h = 7 * _textSize;
    
    while (*text) {
        uint8_t c = *text - 32;
        if (c < 95) {
            // Draw character
            for (uint8_t i = 0; i < 5; i++) {
                uint8_t col = font5x7[c][i];
                for (uint8_t j = 0; j < 7; j++) {
                    if (col & (1 << j)) {
                        fillRect(cursorX + i * _textSize, cursorY + j * _textSize, 
                                _textSize, _textSize, _textColor);
                    } else if (_textBgColor != _textColor) {
                        fillRect(cursorX + i * _textSize, cursorY + j * _textSize, 
                                _textSize, _textSize, _textBgColor);
                    }
                }
            }
        }
        text++;
        cursorX += w;
        if (cursorX > _width - w) {
            cursorX = x;
            cursorY += h;
        }
    }
}

void GC9A01::drawNumber(int32_t num, uint16_t x, uint16_t y) {
    char buffer[12];
    itoa(num, buffer, 10);
    drawString(buffer, x, y);
}

void GC9A01::setRotation(uint8_t rotation) {
    _rotation = rotation & 3;
    
    switch (_rotation) {
        case 0:
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 1:
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
        case 2:
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 3:
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
    }
    
    writeCommand(0x36);
    uint8_t rot;
    switch (_rotation) {
        case 0: rot = 0x08; break;  // Normal - BGR
        case 1: rot = 0x68; break;  // 90 deg
        case 2: rot = 0xC8; break;  // 180 deg
        case 3: rot = 0xA8; break;  // 270 deg
    }
    writeData(rot);
}

void GC9A01::setBacklight(uint8_t brightness) {
    pinMode(_bl, OUTPUT);
    analogWrite(_bl, brightness);
}

// Simple 5x7 font (ASCII 32-126)
const uint8_t font5x7[][5] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x03, 0x00, 0x03, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x07, 0x01}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x3E, 0x41, 0x5D, 0x49, 0x4E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x3A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x26, 0x49, 0x49, 0x49, 0x32}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 'Z'
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // '['
    {0x02, 0x04, 0x08, 0x10, 0x20}, // '\'
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ']'
    {0x04, 0x02, 0x01, 0x02, 0x04}, // '^'
    {0x40, 0x40, 0x40, 0x40, 0x40}, // '_'
    {0x00, 0x01, 0x02, 0x04, 0x00}, // '`'
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 'a'
    {0x7F, 0x50, 0x48, 0x48, 0x30}, // 'b'
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 'c'
    {0x30, 0x48, 0x48, 0x50, 0x7F}, // 'd'
    {0x38, 0x54, 0x54, 0x54, 0x20}, // 'e'
    {0x00, 0x08, 0x7E, 0x09, 0x02}, // 'f'
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // 'g'
    {0x7F, 0x08, 0x08, 0x08, 0x70}, // 'h'
    {0x00, 0x28, 0x7E, 0x40, 0x00}, // 'i'
    {0x00, 0x40, 0xA0, 0xBE, 0x00}, // 'j'
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // 'k'
    {0x00, 0x7F, 0x40, 0x40, 0x00}, // 'l'
    {0x7C, 0x04, 0x78, 0x04, 0x78}, // 'm'
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // 'n'
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 'o'
    {0xFC, 0x10, 0x10, 0x10, 0x60}, // 'p'
    {0x60, 0x10, 0x10, 0x10, 0xFC}, // 'q'
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // 'r'
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 's'
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // 't'
    {0x3C, 0x40, 0x40, 0x40, 0x7C}, // 'u'
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // 'v'
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // 'w'
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 'x'
    {0x04, 0xE0, 0x50, 0x0C, 0x04}, // 'y'
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // 'z'
    {0x00, 0x10, 0x6C, 0x82, 0x00}, // '{'
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // '|'
    {0x00, 0x82, 0x6C, 0x10, 0x00}, // '}'
    {0x08, 0x04, 0x08, 0x10, 0x08}, // '~'
};
