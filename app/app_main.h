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
	Str8 abbreviation;
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
	r32 longestAbbreviationWidth;
	r32 longestAbbreviationWidthFontSize;
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

#define POPUP_DIALOG_CALLBACK_DEF(functionName) void functionName(PopupDialogResult result, struct PopupDialog* dialog, struct PopupDialogButton* selectedButton, void* contextPntr)
typedef POPUP_DIALOG_CALLBACK_DEF(PopupDialogCallback_f);

typedef struct EditableText EditableText;
struct EditableText
{
	Arena* arena;
	uxx maxLength;
	
	bool isFocused;
	bool isMouseHovering;
	uxx mouseHoverIndex;
	bool cursorActive;
	uxx cursorEnd; //this is visually where the cursor is at, not necassarily a larger index than cursorStart
	uxx cursorStart;
	u64 cursorMoveTime; //used to offset the phase of the cursor blinking
	
	Str8 str; //points at strBuffer.items
	VarArray strBuffer; //char
	
	bool textChanged;
};

typedef struct ClayTextbox ClayTextbox;
struct ClayTextbox
{
	Str8 idStr;
	Str8 hintStr;
	u16 clayFontId;
	r32 fontSize;
	
	EditableText edit;
	TextMeasure measure;
	VarArray flowGlyphs; //FontFlowGlyph
	
	bool mainRecChanged;
	rec mainRec;
	v2 textPos;
};

typedef struct AppData AppData;
struct AppData
{
	bool initialized;
	RandomSeries random;
	AppResources resources;
	VarArray fileWatches; //FileWatch
	r32 prevUpdateMs;
	PerfGraph perfGraph;
	bool showPerfGraph;
	AppSettings settings;
	FilePath settingsPath;
	
	Shader mainShader;
	PigFont uiFont;
	r32 uiFontSize;
	r32 uiScale;
	r32 mainFontSize;
	PigFont mainFont;
	Texture icons[AppIcon_Count];
	
	Theme themePresets[PresetTheme_Count];
	PresetTheme currentThemePreset;
	Theme themeOverrides;
	bool needToReloadUserTheme;
	
	u64 numFramesConsecutivelyRendered;
	bool renderedLastFrame;
	
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
	NotificationQueue notificationQueue;
	Texture notificationIconsTexture;
	TooltipRegistry tooltips;
	
	VarArray recentFiles; //RecentFile
	uxx recentFilesWatchId;
	
	VarArray tabs; //FileTab
	uxx currentTabIndex;
	FileTab* currentTab;
	
	ClayTextbox testTextbox;
	u64 testTooltipId;
	
	#if 0
	TOOLINFO tooltipInfo;
	HWND tooltipWindowHandle;
	#endif
	
	//User Options
	// bool smallBtnModeEnabled;
	// bool clipNamesOnLeftSide;
	// bool smoothScrollingEnabled;
	// bool optionTooltipsEnabled;
	bool sleepingDisabled;
	#if DEBUG_BUILD
	bool enableFrameUpdateIndicator;
	#endif
	bool minimalModeEnabled;
	
	SpriteSheet testSheet;
};

#endif //  _APP_MAIN_H
