/**
 * @file lv_conf.h
 * Configuration file for LVGL v9.x
 */

/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
   DISPLAY SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

/* Display resolution */
#define LV_HOR_RES 240
#define LV_VER_RES 240

/*====================
    FONT SETTINGS
 *====================*/
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_FONT_MONTSERRAT_28 1

/*====================
    OPTIONAL FEATURES
 *====================*/
/*Disable unused features to save RAM/flash*/
#define LV_USE_ANIMATION 1
#define LV_USE_SHADOW 0
#define LV_USE_BLEND_MODES 0
#define LV_USE_GRID 0
#define LV_USE_TRANSFORM 1

/*Disable unused widgets - only enable what we use*/
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_ARC 1
#define LV_USE_LINE 1
#define LV_USE_BAR 0
#define LV_USE_SLIDER 0
#define LV_USE_SWITCH 0
#define LV_USE_CHECKBOX 0
#define LV_USE_DROPDOWN 0
#define LV_USE_BTNMTX 0
#define LV_USE_KEYBOARD 0
#define LV_USE_TEXTAREA 0
#define LV_USE_MENU 0
#define LV_USE_TABVIEW 0
#define LV_USE_WIN 0
#define LV_USE_CALENDAR 0
#define LV_USE_CHART 0
#define LV_USE_COLORWHEEL 0
#define LV_USE_IMG 0
#define LV_USE_IMGBTN 0
#define LV_USE_LED 0
#define LV_USE_LIST 0
#define LV_USE_MSGBOX 0
#define LV_USE_ROLLER 0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 0
#define LV_USE_TILEVIEW 0
#define LV_USE_OBJMASK 0
#define LV_USE_DRAW 0
#define LV_USE_GPU 0

/*====================
     Memory settings
 *====================*/
#define LV_MEM_SIZE (48U * 1024U)          /*[bytes]*/

/*====================
    HAL SETTINGS
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
#define LV_DISP_DEF_REFR_PERIOD 30      /*[ms]*/

/*Input device read period in milliseconds*/
#define LV_INDEV_DEF_READ_PERIOD 30     /*[ms]*/

#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

#endif /*LV_CONF_H*/

#endif /*End of "Enable config"*/
