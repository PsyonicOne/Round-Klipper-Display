/**
 * UI Implementation for Round Display
 * Futuristic theme with temperature graphs
 */

#include <Arduino.h>
#include <lvgl.h>
#include <UI.h>
#include <round_klipper_conf.h>
#include <math.h>

// Global UI objects
lv_obj_t* mainScreen = nullptr;

// Temperature display objects
lv_obj_t* hotendTempLabel = nullptr;
lv_obj_t* bedTempLabel = nullptr;
lv_obj_t* hotendArc = nullptr;
lv_obj_t* bedArc = nullptr;

// Animation objects
lv_anim_t animStatus;
lv_anim_t animSeparator;

// Track current print status to detect transitions
static String currentPrintStatus = "";

// Arc objects for animation control
lv_obj_t* statusArc = nullptr;
lv_obj_t* separatorArc = nullptr;

// Current temperatures
float currentHotendTemp = 0;
float currentBedTemp = 0;

// Futuristic color scheme
#define COLOR_BG               0x0A0A1A   // Dark blue-black
#define COLOR_CYAN             0x00FFFF   // Neon cyan
#define COLOR_CYAN_DARK        0x008B8B   // Dark cyan
#define COLOR_HOTEND           0xFF4444   // Red for hotend indicator
#define COLOR_HOTEND_DIM       0x631010   // Dim red for hotend background
#define COLOR_HOTEND_TARGET    0xFF8888   // Light red for hotend target
#define COLOR_BED              0x4488FF   // Blue for bed indicator
#define COLOR_BED_DIM          0x10205a   // Dim blue for bed background
#define COLOR_BED_TARGET       0x88CCFF   // Light blue for bed target
#define COLOR_TEXT             0xFFFFFF   // White
#define COLOR_TEXT_DIM         0x888888   // Gray
#define COLOR_BUTTONS          0x6b9294   // Light blueish
#define COLOR_LOAD             0x7ECFC0   // Pastel green
#define COLOR_UNLOAD           0xFFAB91   // Pastel orange
#define COLOR_RESUME           0x81D4FA   // Resume button color
#define COLOR_RESUME_TEXT      0x022B3A   // Resume text color
// Status colors
#define COLOR_STATUS_COMPLETE  0x00FF00   // Green for complete/standby
#define COLOR_STATUS_PRINTING  0x0088FF   // Blue for printing
#define COLOR_STATUS_PAUSED    0xFFAA00   // Orange for paused
#define COLOR_STATUS_ERROR     0xFF0000   // Red for error/cancelled

// Temperature arc gap from top/bottom of display
#define tempPadding    55           // Gap at top and bottom of temperature arcs
#define arcRadius      120          // Radius of temperature arcs (half of arc size)

// Button callbacks
static void btn_load_cb(lv_event_t *event)
{
    (void)event;
    btnLeftClicked();
}

static void btn_unload_cb(lv_event_t *event)
{
    (void)event;
    btnRightClicked();
}

static void btn_resume_cb(lv_event_t *event)
{
    (void)event;
    btnTopClicked();
}

// Helper function to position an object relative to a reference object
void positionRelativeTo(lv_obj_t *obj, lv_obj_t *reference, lv_align_t align, int16_t offsetX, int16_t offsetY)
{
    lv_obj_update_layout(obj); // Ensure layout is current

    lv_area_t coords;
    lv_obj_get_coords(reference, &coords);

    lv_coord_t newX = coords.x1 + offsetX;
    lv_coord_t newY = coords.y1 + offsetY;

    lv_obj_set_pos(obj, newX, newY);
}

static void anim_size_cb(void *var, int32_t v)
{
    lv_obj_set_size((lv_obj_t *)var, v, v); // Resize the circle
}

static void anim_separator_cb(void *var, int32_t v)
{
    lv_obj_t *arc = (lv_obj_t *)var;
    lv_obj_set_size(arc, v, v); // This changes the arc's size, effectively altering its radius
}

static void anim_status_cb(void *var, int32_t v)
{
    lv_obj_t *arc = (lv_obj_t *)var;
    lv_obj_set_size(arc, v, v); // This changes the arc's size, effectively altering its radius
}

// Update temperature display
void ui_updateTemperatures(float hotend, float bed) {
    currentHotendTemp = hotend;
    currentBedTemp = bed;

    // Update labels
    char tempStr[32];
    snprintf(tempStr, sizeof(tempStr), "%.1f°", hotend);
    lv_label_set_text(hotendTempLabel, tempStr);
    
    snprintf(tempStr, sizeof(tempStr), "%.1f°", bed);
    lv_label_set_text(bedTempLabel, tempStr);

    // Update current temp arcs (always show, regardless of target)
    // Hotend max 280, Bed max 80
    if (hotend > 0) {
        lv_arc_set_value(hotendArc, (int)(hotend / 280.0f * 100));
    } else {
        lv_arc_set_value(hotendArc, 0);
    }
    
    if (bed > 0) {
        lv_arc_set_value(bedArc, (int)(bed / 80.0f * 100));
    } else {
        lv_arc_set_value(bedArc, 0);
    }
}

// Create the main screen
void ui_createMainScreen() {
    mainScreen = lv_screen_active();
    lv_obj_set_style_bg_color(mainScreen, lv_color_hex(COLOR_BG), 0);
    lv_obj_set_style_bg_opa(mainScreen, LV_OPA_COVER, 0);
    lv_obj_remove_flag(mainScreen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Global style for arcs - remove rounded ends
    static lv_style_t style_arc_no_knob;
    lv_style_init(&style_arc_no_knob);
    lv_style_set_arc_rounded(&style_arc_no_knob, false);


    // Top button (Resume) - positioned at top-centre
    lv_obj_t *resumeBtn = lv_btn_create(mainScreen);
    lv_obj_set_size(resumeBtn, 240, 105);
    lv_obj_set_pos(resumeBtn, 0, 0);
    lv_obj_add_event_cb(resumeBtn, btn_resume_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(resumeBtn, lv_color_hex(COLOR_RESUME), 0);
    lv_obj_set_style_bg_opa(resumeBtn, LV_OPA_COVER, 0);
    lv_obj_remove_flag(resumeBtn, LV_OBJ_FLAG_SCROLLABLE);

    // === LOAD/UNLOAD BUTTONS ===
    // Left button
    lv_obj_t* loadBtn = lv_btn_create(mainScreen);
    lv_obj_set_size(loadBtn, 118, 60);
    lv_obj_set_pos(loadBtn, 0, 110);
    lv_obj_add_event_cb(loadBtn, btn_load_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(loadBtn, lv_color_hex(COLOR_LOAD), 0);
    lv_obj_set_style_bg_opa(loadBtn, LV_OPA_COVER, 0);
    lv_obj_remove_flag(loadBtn, LV_OBJ_FLAG_SCROLLABLE);

    // Add label to left button
    lv_obj_t *loadLabel = lv_label_create(loadBtn);
    lv_obj_set_style_text_align(loadLabel, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(loadLabel, BOTTOM_LEFT_BTN_NAME);
    lv_obj_center(loadLabel);
    lv_obj_set_style_text_color(loadLabel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(loadLabel, &lv_font_montserrat_14, 0);
    lv_obj_align(loadLabel, LV_ALIGN_BOTTOM_RIGHT, 5, 5);

    // Right button
    lv_obj_t* unloadBtn = lv_btn_create(mainScreen);
    lv_obj_set_size(unloadBtn, 118, 60);
    lv_obj_set_pos(unloadBtn, 122, 110);
    lv_obj_add_event_cb(unloadBtn, btn_unload_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(unloadBtn, lv_color_hex(COLOR_UNLOAD), 0);
    lv_obj_set_style_bg_opa(unloadBtn, LV_OPA_COVER, 0);
    lv_obj_remove_flag(unloadBtn, LV_OBJ_FLAG_SCROLLABLE);

    // Add label to right button
    lv_obj_t *unloadLabel = lv_label_create(unloadBtn);
    lv_obj_set_style_text_align(unloadLabel, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(unloadLabel, BOTTOM_RIGHT_BTN_NAME);
    lv_obj_center(unloadLabel);
    lv_obj_set_style_text_color(unloadLabel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(unloadLabel, &lv_font_montserrat_14, 0);
    lv_obj_align(unloadLabel, LV_ALIGN_BOTTOM_LEFT, -5, 5);

    // === BLACK SEPARATOR ARC ===
    const int separatorArcDiameter = 262;
    const int separatorArcWidth = 32;
    const int separatorArcPos = 120 - (separatorArcDiameter / 2);
    separatorArc = lv_arc_create(mainScreen);
    lv_obj_set_size(separatorArc, separatorArcDiameter, separatorArcDiameter);
    lv_obj_set_pos(separatorArc, separatorArcPos, separatorArcPos);
    lv_arc_set_rotation(separatorArc, 0);
    lv_arc_set_bg_angles(separatorArc, 90 + tempPadding, 90 - tempPadding);
    lv_arc_set_angles(separatorArc, 90 + tempPadding, 90 + tempPadding);
    lv_obj_set_style_arc_color(separatorArc, lv_color_hex(0x000000), 0);  // Black
    lv_obj_set_style_arc_width(separatorArc, separatorArcWidth, 0);
    lv_obj_remove_flag(separatorArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(separatorArc, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(separatorArc, &style_arc_no_knob, LV_PART_INDICATOR);
    lv_obj_remove_style(separatorArc, NULL, LV_PART_KNOB);
    lv_obj_align(separatorArc, LV_ALIGN_CENTER, 0, 0);

    // === STATUS ARC ===
    const int statusArcDiameter = 216;
    const int statusArcWidth = 4;
    statusArc = lv_arc_create(mainScreen);
    lv_obj_set_size(statusArc, statusArcDiameter, statusArcDiameter);
    lv_arc_set_rotation(statusArc, 0);
    lv_arc_set_bg_angles(statusArc, 90 + tempPadding, 90 - tempPadding);
    lv_arc_set_angles(statusArc, 90 + tempPadding, 90 + tempPadding);
    lv_obj_set_style_arc_color(statusArc, lv_color_hex(0x00FF00), 0); // Green
    lv_obj_set_style_arc_width(statusArc, statusArcWidth, 0);
    lv_obj_remove_flag(statusArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(statusArc, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(statusArc, &style_arc_no_knob, LV_PART_INDICATOR);
    lv_obj_remove_style(statusArc, NULL, LV_PART_KNOB);
    lv_obj_align(statusArc, LV_ALIGN_CENTER, 0, 0);

    // Animation for status arc
    const int animDuration = 200; // ms per direction
    lv_anim_init(&animStatus);
    lv_anim_set_var(&animStatus, statusArc);
    lv_anim_set_values(&animStatus, statusArcDiameter, statusArcDiameter - 8);
    lv_anim_set_time(&animStatus, animDuration);
    lv_anim_set_path_cb(&animStatus, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&animStatus, anim_status_cb);
    lv_anim_set_repeat_count(&animStatus, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_delay(&animStatus, 0);
    lv_anim_set_playback_time(&animStatus, animDuration);

    // Animation for black separator arc
    lv_anim_init(&animSeparator);
    lv_anim_set_var(&animSeparator, separatorArc);
    lv_anim_set_values(&animSeparator, separatorArcDiameter, separatorArcDiameter - 8); 
    lv_anim_set_time(&animSeparator, animDuration);
    lv_anim_set_path_cb(&animSeparator, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&animSeparator, anim_separator_cb);
    lv_anim_set_repeat_count(&animSeparator, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_delay(&animSeparator, 0);
    lv_anim_set_playback_time(&animSeparator, animDuration);

    // === HOTEND ARC ===
    const int arcSize = arcRadius * 2;
    const int arcPos = 120 - arcRadius;  // Center on display

    hotendArc = lv_arc_create(mainScreen);
    lv_obj_set_size(hotendArc, arcSize, arcSize);
    lv_obj_set_pos(hotendArc, arcPos, arcPos);
    lv_arc_set_rotation(hotendArc, 0);
    lv_arc_set_bg_angles(hotendArc, 90 + tempPadding, 270 - 3);
    lv_arc_set_angles(hotendArc, 90, 90);
    lv_obj_set_style_arc_color(hotendArc, lv_color_hex(COLOR_HOTEND_DIM), 0);  // Dim red background
    lv_obj_set_style_arc_color(hotendArc, lv_color_hex(COLOR_HOTEND), LV_PART_INDICATOR); // Bright red for current temp
    lv_obj_set_style_arc_width(hotendArc, 10, 0);
    lv_obj_set_style_arc_width(hotendArc, 10, LV_PART_INDICATOR);
    lv_obj_remove_flag(hotendArc, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hotendArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(hotendArc, &style_arc_no_knob, LV_PART_INDICATOR);
    lv_obj_remove_style(hotendArc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_rounded(hotendArc, true, LV_PART_INDICATOR);  // Rounded indicator ends

    // === BED ARC ===
    bedArc = lv_arc_create(mainScreen);
    lv_obj_set_size(bedArc, arcSize, arcSize);
    lv_obj_set_pos(bedArc, arcPos, arcPos);
    lv_arc_set_mode(bedArc, LV_ARC_MODE_REVERSE);
    lv_arc_set_bg_angles(bedArc, 270 + 3, 90 - tempPadding);
    lv_arc_set_angles(bedArc, 270, 270);
    lv_obj_set_style_arc_color(bedArc, lv_color_hex(COLOR_BED_DIM), 0);  // Dim blue background
    lv_obj_set_style_arc_color(bedArc, lv_color_hex(COLOR_BED), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(bedArc, 10, 0);
    lv_obj_set_style_arc_width(bedArc, 10, LV_PART_INDICATOR);
    lv_obj_remove_flag(bedArc, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(bedArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(bedArc, &style_arc_no_knob, LV_PART_INDICATOR);
    lv_obj_remove_style(bedArc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_rounded(bedArc, true, LV_PART_INDICATOR);  // Rounded indicator ends

    // Hotend temperature - created on mainScreen, positioned relative to hotendTitle
    hotendTempLabel = lv_label_create(mainScreen);
    lv_label_set_text(hotendTempLabel, "--°");
    lv_obj_set_style_text_color(hotendTempLabel, lv_color_hex(COLOR_HOTEND), 0);
    lv_obj_set_style_text_font(hotendTempLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(hotendTempLabel, 87, 215);

    // Bed temperature
    bedTempLabel = lv_label_create(mainScreen);
    lv_label_set_text(bedTempLabel, "--°");
    lv_obj_set_style_text_color(bedTempLabel, lv_color_hex(COLOR_BED), 0);
    lv_obj_set_style_text_font(bedTempLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(bedTempLabel, 130, 215);
    
    const int centerX = 120;
    const int centerY = 120;
    const int textRadius = 105;
    const int resumeRadius = 90;
    int x = 0;
    int y = 0;
    struct CurvedText
    {
        const char *text;
        int startAngle;
        int angleStep;
    };

    CurvedText textCurved[] = {
        {"LOOT", 118, 7},
        {"DEB", 47, 7},
        {RESUME_BTN_NAME, 216, 20},
    };

    for (int b = 0; b < 3; b++)
    {
        const char *text = textCurved[b].text;
        int textLen = strlen(text);

        // Create each character along the arc (forward iteration)
        for (int i = 0; i < textLen; i++)
        {
            int angle = textCurved[b].startAngle + (i * textCurved[b].angleStep);
            float angleRad = (angle * PI) / 180.0f;
            if (textCurved[b].text == RESUME_BTN_NAME) // Special case for RESUME_BTN_NAME to position it at the top
            {
                x = centerX + (int)(resumeRadius * cosf(angleRad));
                y = centerY + (int)(resumeRadius * sinf(angleRad));
            } else {
                x = centerX + (int)(textRadius * cosf(angleRad));
                y = centerY + (int)(textRadius * sinf(angleRad));
            }

            lv_obj_t *lbl = lv_label_create(mainScreen);
            char charStr[2] = {text[i], '\0'};
            lv_label_set_text(lbl, charStr);
            lv_obj_set_pos(lbl, x, y);
            lv_obj_set_style_text_color(lbl, lv_color_hex(COLOR_CYAN), 0);
            // Rotate to follow curve: angle - 90 makes text perpendicular to radius
            if (textCurved[b].text == RESUME_BTN_NAME) // Special case for RESUME_BTN_NAME to position it at the top
            {
                lv_obj_set_style_text_font(lbl, &lv_font_montserrat_28, 0);
                lv_obj_set_style_text_color(lbl, lv_color_hex(COLOR_RESUME_TEXT), 0);
                lv_obj_set_style_transform_angle(lbl, (angle + 96) * 10, 0); // LVGL uses centidegrees (10x)
            }
            else
            {
                lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
                lv_obj_set_style_text_color(lbl, lv_color_hex(COLOR_CYAN), 0);
                lv_obj_set_style_transform_angle(lbl, (angle - 90) * 10, 0); // LVGL uses centidegrees (10x)
            }
        }
    }

    Serial.println("UI created");
}

// Update glow circle colors based on status
void ui_updateStatus(const String& status) {
    // Determine color based on status
    uint32_t baseColor;
    
    if (status == "complete" || status == "standby") {
        // Green for complete/standby
        baseColor = COLOR_STATUS_COMPLETE;
        // Change statusArc color to green
        if (statusArc) lv_obj_set_style_arc_color(statusArc, lv_color_hex(0x00FF00), 0);
        // Stop animations when complete/standby - pass NULL to delete all animations
        Serial.println("Stopping animations from complete/standby status...");
        lv_anim_delete(statusArc, NULL);
        lv_anim_delete(separatorArc, NULL);
        currentPrintStatus = status;
    } else if (status == "printing") {
        // Blue for printing
        baseColor = COLOR_STATUS_PRINTING;
        // Change statusArc color to blue
        if (statusArc) lv_obj_set_style_arc_color(statusArc, lv_color_hex(0x0088FF), 0);
        // Start animations only when first entering printing state
        if (currentPrintStatus != "printing") {
            lv_anim_start(&animStatus);
            lv_anim_start(&animSeparator);
        }
        currentPrintStatus = "printing";
    } else if (status == "paused") {
        // Orange for paused
        baseColor = COLOR_STATUS_PAUSED;
    } else if (status == "error" || status == "cancelled") {
        // Red for error/cancelled
        baseColor = COLOR_STATUS_ERROR;
        // Change statusArc color to red
        if (statusArc) lv_obj_set_style_arc_color(statusArc, lv_color_hex(0xFF0000), 0);
        // Stop animations when error/cancelled - pass NULL to delete ALL animations on the arc objects
        Serial.println("Stopping animations from error/cancelled status...");
        bool del1 = lv_anim_delete(statusArc, NULL);  // NULL deletes all animations on statusArc
        bool del2 = lv_anim_delete(separatorArc, NULL);  // NULL deletes all animations on separatorArc
        // Reset arc sizes to default
        if (statusArc) lv_obj_set_size(statusArc, 216, 216);
        if (separatorArc) lv_obj_set_size(separatorArc, 262, 262);
        currentPrintStatus = status;
    } else {
        // Default cyan
        baseColor = COLOR_CYAN;
    }
}
