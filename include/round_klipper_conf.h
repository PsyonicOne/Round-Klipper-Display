// Custom definitions for Round Screen Klipper Panel
// This file contains pin definitions, color schemes, and other configuration settings for the UI and main program.
//Adjust these settings to customize the behavior and appearance of your round screen panel.

// TFT pin definitions for ESP32-2424S012C-I (AliExpress Round Screen - https://www.aliexpress.com/item/1005010512426009.html)
// If you have a different screen this may need to be modified
#define LCD_CS   10
#define LCD_DC   2
#define LCD_RST  1
#define LCD_SCK  6
#define LCD_MOSI 7

// Touch pin definitions for ESP32-2424S012C-I (AliExpress Round Screen - https://www.aliexpress.com/item/1005010512426009.html)
// If you have a different screen this may need to be modified
#define TOUCH_SDA 4
#define TOUCH_SCL 5
#define TOUCH_I2C_ADDR 0x15

// Moonraker connection settings
#define MOONRAKER_HOST "klipper.local" // Your Klipper Hostname or IP address (klipper.local should work if set up correctly on your network)
#define MOONRAKER_PORT 7125  // Default Moonraker port
#define MOONRAKER_API_KEY "30c54741364a44b693dee7cdefbe7178" // Your Moonraker API key (http://klipper.local/access/api_key should display your key)

// WiFi credentials - CHANGE THESE TO SUIT YOUR WIFI NETWORK
#define WIFI_SSID "Your_WiFi_SSID" // Your WiFi network name (SSID)
#define WIFI_PASSWORD "Your_WiFi_Password" // Your WiFi network password
#define HOST_NAME "Hostname" // Hostname for your device on the network (optional, but can be helpful for identifying it)

// Button names and macros - these are the G-code macros that will be sent to Klipper when the buttons are pressed.
// You can change these to match your macro names in Klipper.
#define RESUME_BTN_NAME "RESUME" // Word displayed on top button (curved at top so will look wrong if modified to something other than 6 characters)
#define RESUME_BTN_MACRO "RESUME" // Macro name to execute when top button is pressed
#define BOTTOM_LEFT_BTN_NAME "LOAD" // Word displayed on bottom left button
#define BOTTOM_LEFT_BTN_MACRO "LOAD_FILAMENT" // Macro name to execute when bottom left button is pressed
#define BOTTOM_RIGHT_BTN_NAME "UNLOAD" // Word displayed on bottom right button
#define BOTTOM_RIGHT_BTN_MACRO "UNLOAD_FILAMENT" // Macro name to execute when bottom right button is pressed

// Screen blanking settings
#define SCREEN_BLANK_TIMEOUT_SECS 120 // Screen will go blank when: state is NOT "printing" AND inactive for this many seconds
#define SCREEN_WAKE_TIMEOUT_SECS 10 // Screen will wake up when inactive for less than this many seconds
#define SCREEN_BACKLIGHT_ON 100 // Backlight brightness level when screen is active (0-255)
#define SCREEN_BACKLIGHT_OFF 0 // Backlight brightness level when screen is blanked (0-255, set to 0 for complete off)
