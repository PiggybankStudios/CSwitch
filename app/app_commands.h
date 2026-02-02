/*
File:   app_commands.h
Author: Taylor Robbins
Date:   02\01\2026
*/

#ifndef _APP_COMMANDS_H
#define _APP_COMMANDS_H

typedef enum AppCommand AppCommand;
enum AppCommand
{
	AppCommand_None = 0,
	AppCommand_ReloadBindings,
	AppCommand_ClosePopupOrMenu,
	AppCommand_OpenFile,
	AppCommand_ReopenRecentFile,
	AppCommand_CloseTab,
	AppCommand_CloseWindow,
	AppCommand_ResetFile,
	AppCommand_ToggleLightMode,
	AppCommand_ToggleSmallButtons,
	AppCommand_ToggleTopmost,
	AppCommand_ToggleClipSide,
	AppCommand_ToggleSmoothScrolling,
	AppCommand_ToggleOptionTooltips,
	AppCommand_ToggleTopbar,
	AppCommand_ToggleSleeping,
	AppCommand_ToggleFrameIndicator,
	AppCommand_OpenCustomTheme,
	AppCommand_ClearCustomTheme,
	AppCommand_ToggleFileReloading,
	AppCommand_NextTab,
	AppCommand_PreviousTab,
	AppCommand_OpenFileMenu,
	AppCommand_OpenViewMenu,
	AppCommand_ResetUiScale,
	AppCommand_IncreaseUiScale,
	AppCommand_DecreaseUiScale,
	AppCommand_TogglePerfGraph,
	AppCommand_ToggleClayDebug,
	AppCommand_ScrollToTop,
	AppCommand_ScrollToBottom,
	AppCommand_ScrollUpPage,
	AppCommand_ScrollDownPage,
	AppCommand_SelectMoveUp,
	AppCommand_SelectMoveDown,
	AppCommand_SelectMoveLeft,
	AppCommand_SelectMoveRight,
	AppCommand_ToggleSelected,
	AppCommand_Count,
};

const char* GetAppCommandStr(AppCommand enumValue)
{
	switch (enumValue)
	{
		case AppCommand_None:                  return "None";
		case AppCommand_ReloadBindings:        return "ReloadBindings";
		case AppCommand_ClosePopupOrMenu:      return "ClosePopupOrMenu";
		case AppCommand_OpenFile:              return "OpenFile";
		case AppCommand_ReopenRecentFile:      return "ReopenRecentFile";
		case AppCommand_CloseTab:              return "CloseTab";
		case AppCommand_CloseWindow:           return "CloseWindow";
		case AppCommand_ResetFile:             return "ResetFile";
		case AppCommand_ToggleLightMode:       return "ToggleLightMode";
		case AppCommand_ToggleSmallButtons:    return "ToggleSmallButtons";
		case AppCommand_ToggleTopmost:         return "ToggleTopmost";
		case AppCommand_ToggleClipSide:        return "ToggleClipSide";
		case AppCommand_ToggleSmoothScrolling: return "ToggleSmoothScrolling";
		case AppCommand_ToggleOptionTooltips:  return "ToggleOptionTooltips";
		case AppCommand_ToggleTopbar:          return "ToggleTopbar";
		case AppCommand_ToggleSleeping:        return "ToggleSleeping";
		case AppCommand_ToggleFrameIndicator:  return "ToggleFrameIndicator";
		case AppCommand_OpenCustomTheme:       return "OpenCustomTheme";
		case AppCommand_ClearCustomTheme:      return "ClearCustomTheme";
		case AppCommand_ToggleFileReloading:   return "ToggleFileReloading";
		case AppCommand_NextTab:               return "NextTab";
		case AppCommand_PreviousTab:           return "PreviousTab";
		case AppCommand_OpenFileMenu:          return "OpenFileMenu";
		case AppCommand_OpenViewMenu:          return "OpenViewMenu";
		case AppCommand_ResetUiScale:          return "ResetUiScale";
		case AppCommand_IncreaseUiScale:       return "IncreaseUiScale";
		case AppCommand_DecreaseUiScale:       return "DecreaseUiScale";
		case AppCommand_TogglePerfGraph:       return "TogglePerfGraph";
		case AppCommand_ToggleClayDebug:       return "ToggleClayDebug";
		case AppCommand_ScrollToTop:           return "ScrollToTop";
		case AppCommand_ScrollToBottom:        return "ScrollToBottom";
		case AppCommand_ScrollUpPage:          return "ScrollUpPage";
		case AppCommand_ScrollDownPage:        return "ScrollDownPage";
		case AppCommand_SelectMoveUp:          return "SelectMoveUp";
		case AppCommand_SelectMoveDown:        return "SelectMoveDown";
		case AppCommand_SelectMoveLeft:        return "SelectMoveLeft";
		case AppCommand_SelectMoveRight:       return "SelectMoveRight";
		case AppCommand_ToggleSelected:        return "ToggleSelected";
		default: return UNKNOWN_STR;
	}
}

bool DoesAppCommandTriggerOnOsLevelKeyRepeatEvents(AppCommand command)
{
	switch (command)
	{
		case AppCommand_NextTab:         return true;
		case AppCommand_PreviousTab:     return true;
		case AppCommand_IncreaseUiScale: return true;
		case AppCommand_DecreaseUiScale: return true;
		case AppCommand_SelectMoveUp:    return true;
		case AppCommand_SelectMoveDown:  return true;
		case AppCommand_SelectMoveLeft:  return true;
		case AppCommand_SelectMoveRight: return true;
		default: return false;
	}
}

void RunAppCommand(AppCommand command);

#endif //  _APP_COMMANDS_H
