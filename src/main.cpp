/**
 * ESP32-2424S012C-I Round LCD - LVGL Graphics + Touch
 * Using GC9A01 display driver + CTouch library
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <lvgl.h>
#include <GC9A01.h>
#include <lv_drivers/display/GC9A01_lvgl.h>
#include <CTouch.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <UI.h>
#include <round_klipper_conf.h>

#include <WiFi.h>
#include <esp_task_wdt.h>

// Display object
GC9A01 tft(LCD_SCK, LCD_MOSI, LCD_CS, LCD_DC, LCD_RST);

// Touch object
CTouch touch(TOUCH_SDA, TOUCH_SCL, TOUCH_I2C_ADDR);

// Moonraker client
WebSocketsClient MoonrakerWS;

// Info from Moonraker
static uint32_t lastTempUpdate;
static float hotendTemp;
static float bedTemp;
static String state;

// Screen blanking state
static bool screenBlanked = false;
static bool touchWasActive = false;
static uint32_t lastScreenStateCheck = 0;
static uint32_t lastTouchWakeCheck = 0;

// LVGL input device
static lv_indev_t *indev_touch = nullptr;

// LVGL display buffer - smaller for memory efficiency and bulk transfer
static lv_color_t buf1[240 * 40];  // 1/6 screen height
static lv_color_t buf2[240 * 40];

// Global LVGL objects
lv_display_t *display = nullptr;

// Simple JSON parser for Moonraker responses
float parseMoonrakerTemp(const char *json, const char *object)
{
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\":{\"temperature\":", object);

    char *ptr = strstr(json, searchStr);
    if (ptr)
    {
        ptr += strlen(searchStr);
        return atof(ptr);
    }
    return -1.0f;
}

String parseMoonrakerString(const char *json, const char *object, const char *field)
{
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\":{\"%s\":\"", object, field);

    char *ptr = strstr(json, searchStr);
    if (ptr)
    {
        ptr += strlen(searchStr);
        char *endPtr = strstr(ptr, "\"");
        if (endPtr)
        {
            return String(ptr).substring(0, endPtr - ptr);
        }
    }
    return "";
}

// Send G-code command to Moonraker via WebSocket
void sendGcodeCommand(const char *gcode)
{
    char json[256];
    snprintf(json, sizeof(json),
             "{\"jsonrpc\":\"2.0\",\"method\":\"printer.gcode.script\",\"params\":{\"script\":\"%s\"},\"id\":1}",
             gcode);
    MoonrakerWS.sendTXT(json);
    Serial.printf("Sent G-code: %s\n", gcode);
}

// Button action functions (called from UI)
extern "C"
{
    void btnLeftClicked()
    {
        sendGcodeCommand(BOTTOM_RIGHT_BTN_MACRO);
    }

    void btnRightClicked()
    {
        sendGcodeCommand(BOTTOM_LEFT_BTN_MACRO);
    }

    void btnTopClicked()
    {
        sendGcodeCommand(RESUME_BTN_MACRO);
    }
}

// WebSocket event handler
void MoonrakerEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("Moonraker: Disconnected");
            break;
        case WStype_CONNECTED:
            Serial.println("Moonraker: Connected");
            // Subscribe to printer objects
            MoonrakerWS.sendTXT("{\"jsonrpc\":\"2.0\",\"method\":\"printer.objects.subscribe\",\"params\":{\"objects\":{\"extruder\":null,\"heater_bed\":null,\"print_stats\":null,\"toolhead\":null}},\"id\":1}");
            
            // Also query print_stats immediately to get initial state
            MoonrakerWS.sendTXT("{\"jsonrpc\":\"2.0\",\"method\":\"printer.objects.query\",\"params\":{\"objects\":[\"print_stats\"]},\"id\":2}");
            break;
        case WStype_TEXT: {
            Serial.printf("Moonraker: Received: %s\n", payload);
            
            // Parse temperatures from the message
            float temp = parseMoonrakerTemp((const char*)payload, "extruder");
            if (temp >= 0.0f) {
                hotendTemp = temp;
                Serial.printf("Hotend temp: %.1f\n", hotendTemp);
            }
            
            // Parse bed temperature
            temp = parseMoonrakerTemp((const char*)payload, "heater_bed");
            if (temp >= 0.0f) {
                bedTemp = temp;
                Serial.printf("Bed temp: %.1f\n", bedTemp);
            }
            
            // Parse print state - Moonraker sends print_stats like:
            // "print_stats":{"state":"printing",...}
            // Find print_stats, then find state field within it
            const char* psPtr = strstr((const char*)payload, "\"print_stats\":{");
            if (psPtr) {
                // Now search for "state":" within print_stats
                const char* statePtr = strstr(psPtr, "\"state\":\"");
                if (statePtr) {
                    statePtr += 9; // Skip past "state":" (9 chars)
                    char* endPtr = strstr(statePtr, "\"");
                    if (endPtr) {
                        state = String(statePtr).substring(0, endPtr - statePtr);
                        Serial.printf("Print state: %s\n", state.c_str());
                    }
                }
            }
            break;
        }
        case WStype_ERROR:
            Serial.println("Moonraker: Error");
            break;
        default:
            break;
    }
}

// LVGL touch input device read callback
static void touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
    int x, y;
    if (touch.readTouch(x, y)) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        touchWasActive = true;  // Track touch activity for screen wake
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== LVGL Round Display (Graphics and Touch) ===");
    Serial.flush();
    
    // Initialize watchdog timer to prevent random reboots (disabled for debugging)
    esp_task_wdt_init(20, true);  // 20 second timeout
    esp_task_wdt_add(NULL);
    
    // Initialize SPI with higher frequency for better performance
    SPI.begin(LCD_SCK, -1, LCD_MOSI, LCD_CS);
    SPI.setFrequency(20000000);  // 20MHz for faster rendering
    
    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.setBacklight(100);
    tft.fillScreen(GC9A01::TFT_BLACK);
    Serial.println("Display initialized");

    // Initialize touch controller
    touch.begin();
    touch.setDefaultCalibration();
    touch.setRotation(CTouchRotation::ROTATION_0);  // Adjust rotation for correct touch mapping
    Serial.println("Touch initialized");
    
    // Initialize LVGL
    lv_init();

    // Initialize display driver
    gc9a01_lvgl_init(&tft);

    // Create display - using partial render mode
    display = lv_display_create(240, 240);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, gc9a01_lvgl_flush);
    Serial.println("LVGL display created");
    
    // Create LVGL input device for touch
    indev_touch = lv_indev_create();
    lv_indev_set_type(indev_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touch, touch_read_cb);
    Serial.println("LVGL touch input device created");
    
    Serial.println("Ready!");
    
    // WiFi disabled for testing
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_17dBm);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int wifiAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifiAttempts < 30) {
        delay(500);
        Serial.print(".");
        wifiAttempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.print("WiFi: ");
        Serial.println(WiFi.localIP());
        
        // Initialize Moonraker WebSocket
        MoonrakerWS.begin(MOONRAKER_HOST, MOONRAKER_PORT, "/websocket");
        MoonrakerWS.onEvent(MoonrakerEvent);
        MoonrakerWS.setReconnectInterval(5000);
        
        // Add API key header if configured
        #ifdef MOONRAKER_API_KEY
        if (strlen(MOONRAKER_API_KEY) > 0) {
            MoonrakerWS.setExtraHeaders("X-API-Key: " MOONRAKER_API_KEY);
        }
        #endif
        
        Serial.println("Moonraker WebSocket initializing...");
    } else {
        Serial.println(" WiFi failed");
    }
    
    // Create UI
    ui_createMainScreen();

    // Invalidate the entire screen to trigger initial render
    lv_obj_invalidate(lv_screen_active());

    lastTempUpdate = millis();
    hotendTemp = 0.0;
    bedTemp = 0.0;
    state = "unknown";
    // lv_anim_delete(&animStatus);
    // lv_anim_delete(&animSeparator);
}

void loop() {
    
    // Update temperatures every 2 seconds (only if screen is not blanked)
    if (millis() - lastTempUpdate > 2000) {
        lastTempUpdate = millis();
        
        // Only update UI when screen is not blanked
        if (!screenBlanked) {
            ui_updateTemperatures(hotendTemp, bedTemp);
            ui_updateStatus(state);
        }
        
        Serial.print("Updated: ");
        Serial.print(hotendTemp);
        Serial.print("° (hotend), ");
        Serial.print(bedTemp);
        Serial.print("° (bed), status: ");
        Serial.println(state);
    }

    // Handle LVGL tasks
    lv_tick_inc(1);
    lv_timer_handler();
    
    // Screen blanking logic
    // Check every 500ms to avoid excessive processing
    if (millis() - lastScreenStateCheck > 500) {
        lastScreenStateCheck = millis();
        
        uint32_t inactiveTime = lv_disp_get_inactive_time(lv_display_get_default());
        uint32_t inactiveSeconds = inactiveTime / 1000; // Convert ms to seconds
        
        // Check if we should blank the screen
        // Blank if: state is NOT "printing" AND inactive > BLANK_TIMEOUT
        bool isPrinting = (state == "printing");
        
        if (!screenBlanked && !isPrinting && inactiveSeconds > SCREEN_BLANK_TIMEOUT_SECS) {
            // Blank the screen
            tft.setBacklight(SCREEN_BACKLIGHT_OFF);
            screenBlanked = true;
            Serial.println("Screen blanked (inactive)");
        }
        else if (screenBlanked) {
            // Screen is blanked - check if touch should wake it
            // Only wake on actual touch activity, not just UI updates
            if (touchWasActive) {
                // Reset touch flag
                touchWasActive = false;
                // Only wake if touch happened recently (within last second)
                if (inactiveSeconds < 1) {
                    tft.setBacklight(SCREEN_BACKLIGHT_ON);
                    screenBlanked = false;
                    Serial.println("Screen woken up (touch)");
                }
            }
        }
    }
    
    // Reset touch flag after a delay to avoid stale touch events
    if (millis() - lastTouchWakeCheck > 2000) {
        lastTouchWakeCheck = millis();
        touchWasActive = false;
    }

    // Handle Moonraker WebSocket
    MoonrakerWS.loop();

    // feed the dog
    esp_task_wdt_reset();
    
    delay(1);
}
