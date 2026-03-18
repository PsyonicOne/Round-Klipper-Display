/**
 * @file GC9A01_lvgl.cpp
 * LVGL display driver implementation for GC9A01 round LCD
 * Optimized using bulk SPI transfer
 */

#include <lv_drivers/display/GC9A01_lvgl.h>
#include <GC9A01.h>

// Global pointer to the display object
static GC9A01* s_tft = nullptr;

void gc9a01_lvgl_init(GC9A01* tft) {
    s_tft = tft;
}

void gc9a01_lvgl_flush(lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
    if (s_tft == nullptr || display == nullptr) {
        return;
    }
    
    // Calculate the number of pixels
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    uint32_t pixel_count = w * h;
    
    // Set the address window and prepare for data
    s_tft->setAddrWindow(area->x1, area->y1, area->x2, area->y2);
    
    // Push all pixels at once (faster than pixel-by-pixel)
    uint16_t* pixels = (uint16_t*)px_map;
    s_tft->pushColors(pixels, pixel_count);
    
    // Tell LVGL flushing is done
    lv_display_flush_ready(display);
}
