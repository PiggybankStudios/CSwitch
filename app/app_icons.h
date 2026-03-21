/*
File:   app_icons.h
Author: Taylor Robbins
Date:   03\04\2025
*/

#ifndef _APP_ICONS_H
#define _APP_ICONS_H

typedef enum AppIcon AppIcon;
enum AppIcon
{
	AppIcon_None,
	AppIcon_OpenFile,
	AppIcon_OpenRecent,
	AppIcon_Trash,
	AppIcon_ResetFile,
	AppIcon_CloseFile,
	AppIcon_SmallBtn,
	AppIcon_TopmostEnabled,
	AppIcon_TopmostDisabled,
	AppIcon_ClipLeft,
	AppIcon_ClipRight,
	AppIcon_SmoothScroll,
	AppIcon_Tooltip,
	AppIcon_Topbar,
	AppIcon_Debug,
	AppIcon_CloseWindow,
	AppIcon_LightDark,
	AppIcon_StarFile,
	AppIcon_Count,
};
const char* GetAppIconStr(AppIcon enumValue)
{
	switch (enumValue)
	{
		case AppIcon_None:                return "None";
		case AppIcon_OpenFile:            return "OpenFile";
		case AppIcon_OpenRecent:          return "OpenRecent";
		case AppIcon_Trash:               return "Trash";
		case AppIcon_ResetFile:           return "ResetFile";
		case AppIcon_CloseFile:           return "CloseFile";
		case AppIcon_SmallBtn:            return "SmallBtn";
		case AppIcon_TopmostEnabled:      return "TopmostEnabled";
		case AppIcon_TopmostDisabled:     return "TopmostDisabled";
		case AppIcon_ClipLeft:            return "ClipLeft";
		case AppIcon_ClipRight:           return "ClipRight";
		case AppIcon_SmoothScroll:        return "SmoothScroll";
		case AppIcon_Tooltip:             return "Tooltip";
		case AppIcon_Topbar:              return "Topbar";
		case AppIcon_Debug:               return "Debug";
		case AppIcon_CloseWindow:         return "CloseWindow";
		case AppIcon_LightDark:           return "LightDark";
		case AppIcon_StarFile:            return "StarFile";
		default: return "Unknown";
	}
}
const char* GetAppIconSheetCellName(AppIcon enumValue)
{
	switch (enumValue)
	{
		case AppIcon_OpenFile:            return "open_file";
		case AppIcon_OpenRecent:          return "recent";
		case AppIcon_Trash:               return "trash";
		case AppIcon_ResetFile:           return "reset_file";
		case AppIcon_CloseFile:           return "close_file";
		case AppIcon_SmallBtn:            return "small_btn";
		case AppIcon_TopmostEnabled:      return "topmost_on";
		case AppIcon_TopmostDisabled:     return "topmost_off";
		case AppIcon_ClipLeft:            return "clip_left";
		case AppIcon_ClipRight:           return "clip_right";
		case AppIcon_SmoothScroll:        return "smooth_scroll";
		case AppIcon_Tooltip:             return "tooltip";
		case AppIcon_Topbar:              return "topbar";
		case AppIcon_Debug:               return "debug";
		case AppIcon_CloseWindow:         return "close_window";
		case AppIcon_LightDark:           return "light_dark";
		case AppIcon_StarFile:            return "star_file";
		default: return nullptr;
	}
}

#endif //  _APP_ICONS_H
