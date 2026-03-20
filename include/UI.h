/**
 * UI functions for Round Display
 * LVGL screen creation and management
 */

#ifndef UI_H
#define UI_H

#include <lvgl.h>

// Forward declarations

// Screen creation functions
void ui_createMainScreen();
void ui_createSettingsScreen();

// Update functions
void ui_updateTemperatures(float hotend, float bed);
void ui_updateStatus(const String& status);

// Connection status functions
void ui_setConnecting();
void ui_setConnected();
void ui_setConnectionFailed();

// Button action functions (implemented in main.cpp)
extern "C" {
    void btnLeftClicked();
    void btnRightClicked();
    void btnTopClicked();
}

// Global LVGL objects
extern lv_obj_t* mainScreen;
extern lv_anim_t animStatus;
extern lv_anim_t animSeparator;

// Circle animation setters
extern "C" void setCirclePointers(lv_obj_t* outer, lv_obj_t* inner);

// Helper functions
extern "C" void positionRelativeTo(lv_obj_t* obj, lv_obj_t* reference, lv_align_t align, int16_t offsetX, int16_t offsetY);

#endif // UI_H
