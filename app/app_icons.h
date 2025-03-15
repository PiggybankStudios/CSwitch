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
	AppIcon_TopmostEnabled,
	AppIcon_TopmostDisabled,
	AppIcon_ClipLeft,
	AppIcon_ClipRight,
	AppIcon_SmoothScroll,
	AppIcon_Tooltip,
	AppIcon_Topbar,
	AppIcon_Debug,
	AppIcon_CloseWindow,
	AppIcon_Count,
};
const char* GetAppIconStr(AppIcon enumValue)
{
	switch (enumValue)
	{
		case AppIcon_None:            return "None";
		case AppIcon_OpenFile:        return "OpenFile";
		case AppIcon_OpenRecent:      return "OpenRecent";
		case AppIcon_Trash:           return "Trash";
		case AppIcon_ResetFile:       return "ResetFile";
		case AppIcon_CloseFile:       return "CloseFile";
		case AppIcon_TopmostEnabled:  return "TopmostEnabled";
		case AppIcon_TopmostDisabled: return "TopmostDisabled";
		case AppIcon_ClipLeft:        return "ClipLeft";
		case AppIcon_ClipRight:       return "ClipRight";
		case AppIcon_SmoothScroll:    return "SmoothScroll";
		case AppIcon_Tooltip:         return "Tooltip";
		case AppIcon_Topbar:          return "Topbar";
		case AppIcon_Debug:           return "Debug";
		case AppIcon_CloseWindow:     return "CloseWindow";
		default: return "Unknown";
	}
}
const char* GetAppIconPath(AppIcon enumValue)
{
	switch (enumValue)
	{
		case AppIcon_OpenFile:        return "resources/image/open_file.png";
		case AppIcon_OpenRecent:      return "resources/image/recent.png";
		case AppIcon_Trash:           return "resources/image/trash.png";
		case AppIcon_ResetFile:       return "resources/image/reset_file.png";
		case AppIcon_CloseFile:       return "resources/image/close_file.png";
		case AppIcon_TopmostEnabled:  return "resources/image/topmost_on.png";
		case AppIcon_TopmostDisabled: return "resources/image/topmost_off.png";
		case AppIcon_ClipLeft:        return "resources/image/clip_left.png";
		case AppIcon_ClipRight:       return "resources/image/clip_right.png";
		case AppIcon_SmoothScroll:    return "resources/image/smooth_scroll.png";
		case AppIcon_Tooltip:         return "resources/image/tooltip.png";
		case AppIcon_Topbar:          return "resources/image/topbar.png";
		case AppIcon_Debug:           return "resources/image/debug.png";
		case AppIcon_CloseWindow:     return "resources/image/close_window.png";
		default: return nullptr;
	}
}

#endif //  _APP_ICONS_H
