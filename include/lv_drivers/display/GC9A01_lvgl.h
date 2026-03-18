/**
 * @file GC9A01_lvgl.h
 * LVGL display driver for GC9A01 round LCD
 */

#ifndef GC9A01_LVGL_H
#define GC9A01_LVGL_H

#include <lvgl.h>
#include <GC9A01.h>

// Initialize the display driver with the GC9A01 instance
void gc9a01_lvgl_init(GC9A01* tft);

// Flush callback function for LVGL
void gc9a01_lvgl_flush(lv_display_t* display, const lv_area_t* area, uint8_t* px_map);

#endif /* GC9A01_LVGL_H */
