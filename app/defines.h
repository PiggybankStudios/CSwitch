/*
File:   defines.h
Author: Taylor Robbins
Date:   02\25\2025
*/

#ifndef _DEFINES_H
#define _DEFINES_H

#define NUM_FRAMES_BEFORE_SLEEP 8 //frames
#define TOPBAR_ICONS_SIZE  16 //px
#define TOPBAR_ICONS_PADDING  8 //px

#define RECENT_FILES_SAVE_FILEPATH "recent_files.txt"
#define RECENT_FILES_MAX_LENGTH    16 //files
#define SETTINGS_FILENAME          "settings.txt"

#if 1
#define UI_FONT_PATH  "resources/font/Geologica-Regular.ttf"
#else
#define UI_FONT_NAME  "Consolas"
#endif
#define UI_FONT_STYLE FontStyleFlag_None

#if 1
#define MAIN_FONT_PATH  "resources/font/Geologica-Bold.ttf"
#else
#define MAIN_FONT_NAME  "Consolas"
#endif
#define MAIN_FONT_STYLE FontStyleFlag_Bold

#define MIN_UI_FONT_SIZE       9
#define DEFAULT_UI_FONT_SIZE   14
#define MAIN_TO_UI_FONT_RATIO  1.3f //Default main font size will be 18

#define NOTIFICATION_ICONS_TEXTURE_PATH "resources/image/notifications_2x2.png"
#define NOTIFICATION_ICONS_SIZE         24 //px

#define DEFAULT_WINDOW_SIZE        MakeV2(360, 500)
#define MIN_WINDOW_SIZE            MakeV2(50, 50)
#define SCROLLBAR_WIDTH            10 //px (at uiScale=1.0f)

#define OPTION_UI_GAP                    2 //px (at uiScale=1.0f)
#define OPTIONS_SMOOTH_SCROLLING_DIVISOR 5 //divisor
#define LINE_BREAK_EXTRA_UI_GAP          4 //px (at uiScale=1.0f)
#define MAX_LINE_BREAKS_CONSIDERED       4 //lines

#define CHECK_RECENT_FILES_WRITE_TIME_PERIOD 1000 //ms
#define RECENT_FILES_RELOAD_DELAY            100 //ms

#define CHECK_FILE_WRITE_TIME_PERIOD 300 //ms
#define FILE_RELOAD_DELAY            100 //ms

#define DEFAULT_THEME_PRESET PresetTheme_Dark

#define SMALL_BTN_PADDING_X  8 //px
#define SMALL_BTN_PADDING_Y  8 //px
#define SMALL_BTN_MARGIN     2 //px

#define WIN32_TOOLTIP_ID 100 //arbitrary ID

#define POPUP_OPEN_ANIM_TIME  75 //ms
#define POPUP_CLOSE_ANIM_TIME 50 //ms
#define POPUP_MIN_WIDTH       300 //px
#define POPUP_MIN_HEIGHT      100 //px

#define MAX_NOTIFICATIONS                  16
#define DEFAULT_NOTIFICATION_TIME          5*1000 //ms
#define NOTIFICATION_BETWEEN_MARGIN        3 //px
#define NOTIFICATION_PADDING               8 //px
#define NOTIFICATION_MOVE_LAG              3 //divisor
#define NOTIFICATION_APPEAR_ANIM_TIME      200 //ms
#define NOTIFICATION_DISAPPEAR_ANIM_TIME   300 //ms
#define NOTIFICATION_SCREEN_MARGIN_RIGHT   4 //px
#define NOTIFICATION_SCREEN_MARGIN_BOTTOM  4 //px
#define NOTIFICATION_ICON_SIZE             32 //px
#define NOTIFICATION_AUTO_DISMISS_SCREEN_HEIGHT_PERCENT 0.5f //percent of screen height

#endif //  _DEFINES_H
