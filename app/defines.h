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

#define UI_FONT_NAME  "Consolas"
#define UI_FONT_SIZE  14
#define UI_FONT_STYLE FontStyleFlag_None

#define MAIN_FONT_NAME  "Consolas"
#define MAIN_FONT_SIZE  18
#define MAIN_FONT_STYLE FontStyleFlag_Bold

#define DEFAULT_WINDOW_SIZE        NewV2(360, 500)
#define MIN_WINDOW_SIZE            NewV2(50, 50)
#define TOPBAR_HEIGHT              24 //px
#define OPEN_RECENT_DROPDOWN_WIDTH 300 //px
#define SCROLLBAR_WIDTH            10 //px

#define OPTION_UI_GAP                    2 //px
#define OPTIONS_SMOOTH_SCROLLING_DIVISOR 5 //divisor
#define LINE_BREAK_EXTRA_UI_GAP          4 //px
#define MAX_LINE_BREAKS_CONSIDERED       4 //lines

#define CHECK_RECENT_FILES_WRITE_TIME_PERIOD 1000 //ms
#define RECENT_FILES_RELOAD_DELAY            100 //ms

#define CHECK_FILE_WRITE_TIME_PERIOD 300 //ms
#define FILE_RELOAD_DELAY            100 //ms

#define TEXT_WHITE           NewColor(221, 222, 224, 255) //FFDDDEE0
#define TEXT_LIGHT_GRAY      NewColor(175, 177, 179, 255) //FFAFB1B3
#define TEXT_GRAY            NewColor(107, 112, 120, 255) //FF6B7078
#define BACKGROUND_BLACK     NewColor(25, 27, 28, 255)    //FF191B1C
#define BACKGROUND_DARK_GRAY NewColor(31, 34, 35, 255)    //FF1F2223
#define BACKGROUND_GRAY      NewColor(39, 42, 43, 255)    //FF272A2B
#define OUTLINE_GRAY         NewColor(52, 58, 59, 255)    //FF343A3B
#define HOVERED_BLUE         NewColor(16, 60, 76, 255)    //FF103C4C
#define SELECTED_BLUE        NewColor(0, 121, 166, 255)   //FF0079A6
#define ERROR_RED            NewColor(255, 102, 102, 255) //FFFF6666

#define WIN32_TOOLTIP_ID 100 //arbitrary ID

#define DEFAULT_TOOLTIP_DELAY 750 //ms
#define OPTION_NAME_TOOLTIP_DELAY 1000 //ms
#define TOOLTIP_MAX_MOVE_DIST 5 //px
#define TOOLTIP_TEXT_MARGIN   NewV2(8, 4) //px
#define TOOLTIP_MOUSE_UP_OFFSET    5 //px
#define TOOLTIP_MOUSE_DOWN_OFFSET  20 //px TODO: Somehow we should ask the OS how tall the cursor is and offset but that much plus a little

#endif //  _DEFINES_H
