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
	// TODO: Add tooltips to all the options!
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

typedef struct AppData AppData;
struct AppData
{
	bool initialized;
	RandomSeries random;
	AppResources resources;
	VarArray fileWatches; //FileWatch
	
	Shader mainShader;
	Font uiFont;
	Font mainFont;
	Texture icons[AppIcon_Count];
	
	u64 numFramesConsecutivelyRendered;
	
	ClayUIRenderer clay;
	u16 clayUiFontId;
	u16 clayMainFontId;
	bool isFileMenuOpen;
	bool isWindowMenuOpen;
	bool isOpenRecentSubmenuOpen;
	
	bool isFileOpen;
	Str8 filePath;
	Str8 fileContents;
	uxx fileWatchId;
	VarArray fileOptions; //FileOption
	u64 filePathTooltipId;
	
	VarArray recentFiles; //RecentFile
	uxx recentFilesWatchId;
	
	#if 0
	TOOLINFO tooltipInfo;
	HWND tooltipWindowHandle;
	#endif
	
	u64 nextTooltipId;
	VarArray tooltipRegions;
	TooltipState tooltip;
	
	//User Options
	bool clipNamesOnLeftSide;
};

#endif //  _APP_MAIN_H
