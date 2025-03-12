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
	rec mainRec;
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
	uxx fileWatchId;
	VarArray fileOptions; //FileOption
	ScrollbarInteractionState scrollbarState;
};

typedef struct AppData AppData;
struct AppData
{
	bool initialized;
	RandomSeries random;
	AppResources resources;
	VarArray fileWatches; //FileWatch
	
	Shader mainShader;
	Font uiFont;
	r32 uiFontSize;
	r32 uiScale;
	r32 mainFontSize;
	Font mainFont;
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
	bool wasClayScrollingPrevFrame;
	
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
