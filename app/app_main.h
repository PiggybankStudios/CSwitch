/*
File:   app_main.h
Author: Taylor Robbins
Date:   02\25\2025
*/

#ifndef _APP_MAIN_H
#define _APP_MAIN_H

typedef enum FileOptionType FileOptionType;
enum FileOptionType
{
	FileOptionType_None = 0,
	FileOptionType_Bool,
	FileOptionType_CommentDefine,
	FileOptionType_Count,
};
const char* GetFileOptionTypeStr(FileOptionType enumValue)
{
	switch (enumValue)
	{
		case FileOptionType_None:          return "None";
		case FileOptionType_Bool:          return "Bool";
		case FileOptionType_CommentDefine: return "CommentDefine";
		default: return "Unknown";
	}
}

typedef struct FileOption FileOption;
struct FileOption
{
	Str8 name;
	FileOptionType type;
	u64 numEmptyLinesAfter;
	Str8 valueStr;
	uxx fileContentsStartIndex;
	uxx fileContentsEndIndex;
	u64 tooltipId;
	union
	{
		bool valueBool;
		bool isUncommented;
		i32 valueI32;
		r32 valueR32;
	};
};

typedef struct RecentFile RecentFile;
struct RecentFile
{
	Str8 path;
	bool fileExists;
	u64 tooltipId;
};

typedef struct FileWatch FileWatch;
struct FileWatch
{
	uxx id; //this is just the index+1 (meaning 0 can be used as an "invalid" id)
	uxx checkPeriod;
	uxx numReferences;
	FilePath path;
	FilePath fullPath;
	bool changed;
	u64 lastCheck;
	u64 lastChangeTime;
	bool fileExists;
	bool gotWriteTime;
	OsFileWriteTime writeTime;
};

typedef struct TooltipRegion TooltipRegion;
struct TooltipRegion
{
	u64 id;
	u64 delay;
	bool enabled;
	u64 layer; //higher numbers have higher priority when disambiguating overlapping regions
	Str8 displayStr;
	Arena* arena;
	
	rec fixedRec;
	ClayId clayId;
	ClayId clayContainerId;
};

typedef struct TooltipState TooltipState;
struct TooltipState
{
	Arena* arena;
	u64 lastMouseMoveTime;
	v2 lastMouseMovePos;
	u64 lastMouseMoveRegionId;
	bool isOpen;
	u64 regionId;
	rec mainRec;
	v2 focusPos;
	Str8 displayStr;
	TextMeasure displayStrMeasure;
};

typedef struct ScrollbarInteractionState ScrollbarInteractionState;
struct ScrollbarInteractionState
{
	bool isDragging;
	bool isDraggingSmooth;
	v2 grabOffset;
};

typedef struct FileTab FileTab;
struct FileTab
{
	Str8 filePath;
	Str8 fileContents;
	Str8 originalFileContents;
	bool isFileChangedFromOriginal;
	uxx fileWatchId;
	VarArray fileOptions; //FileOption
	ScrollbarInteractionState scrollbarState;
};

typedef enum PopupDialogResult PopupDialogResult;
enum PopupDialogResult
{
	PopupDialogResult_None = 0,
	PopupDialogResult_Yes,
	PopupDialogResult_No,
	PopupDialogResult_Closed,
	PopupDialogResult_Count,
};
const char* GetPopupDialogResultStr(PopupDialogResult enumValue)
{
	switch (enumValue)
	{
		case PopupDialogResult_None:   return "None";
		case PopupDialogResult_Yes:    return "Yes";
		case PopupDialogResult_No:     return "No";
		case PopupDialogResult_Closed: return "Closed";
		case PopupDialogResult_Count:  return "Count";
		default: return "Unknown";
	}
}

typedef struct PopupDialogButton PopupDialogButton;
struct PopupDialogButton
{
	Arena* arena;
	u64 id;
	PopupDialogResult result;
	Str8 displayStr;
	Color32 color;
};

typedef struct PopupDialog PopupDialog;
struct PopupDialog
{
	Arena* arena;
	bool isOpen;
	bool isVisible; //stays true for some amount of time after closing the dialog, resources are freed once this is false
	
	u64 openTime;
	u64 closeTime;
	PopupDialogResult result;
	Str8 messageStr;
	VarArray buttons; //PopupDialogButton
	
	void* callback; //PopupDialogCallback_f
	void* callbackContext;
};

typedef struct Notification Notification;
struct Notification
{
	Arena* arena;
	u64 id;
	u64 spawnTime;
	u64 duration;
	Str8 messageStr;
	r32 currentOffsetY;
	r32 gotoOffsetY;
	DbgLevel level;
};

typedef struct NotificationQueue NotificationQueue;
struct NotificationQueue
{
	Arena* arena;
	u64 nextId;
	VarArray notifications; //Notification
};

#define POPUP_DIALOG_CALLBACK_DEF(functionName) void functionName(PopupDialogResult result, struct PopupDialog* dialog, struct PopupDialogButton* selectedButton, void* contextPntr)
typedef POPUP_DIALOG_CALLBACK_DEF(PopupDialogCallback_f);

typedef struct AppData AppData;
struct AppData
{
	bool initialized;
	RandomSeries random;
	AppResources resources;
	VarArray fileWatches; //FileWatch
	
	Shader mainShader;
	PigFont uiFont;
	r32 uiFontSize;
	r32 uiScale;
	r32 mainFontSize;
	PigFont mainFont;
	Texture icons[AppIcon_Count];
	
	u64 numFramesConsecutivelyRendered;
	
	ClayUIRenderer clay;
	u16 clayUiFontId;
	u16 clayMainFontId;
	bool isFileMenuOpen;
	bool keepFileMenuOpenUntilMouseOver;
	bool isViewMenuOpen;
	bool keepViewMenuOpenUntilMouseOver;
	bool isOpenRecentSubmenuOpen;
	bool keepOpenRecentSubmenuOpenUntilMouseOver;
	bool wasClayScrollingPrevFrame;
	
	PopupDialog popup;
	NotificationQueue notifications;
	
	VarArray recentFiles; //RecentFile
	uxx recentFilesWatchId;
	
	VarArray tabs; //FileTab
	uxx currentTabIndex;
	FileTab* currentTab;
	
	#if 0
	TOOLINFO tooltipInfo;
	HWND tooltipWindowHandle;
	#endif
	
	u64 nextTooltipId;
	VarArray tooltipRegions;
	TooltipState tooltip;
	u64 filePathTooltipId;
	
	//User Options
	bool clipNamesOnLeftSide;
	bool smoothScrollingEnabled;
	bool optionTooltipsEnabled;
	bool sleepingDisabled;
	#if DEBUG_BUILD
	bool enableFrameUpdateIndicator;
	#endif
	bool minimalModeEnabled;
};

#endif //  _APP_MAIN_H
