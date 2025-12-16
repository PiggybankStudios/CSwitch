/*
File:   app_theme.h
Author: Taylor Robbins
Date:   12\10\2025
*/

#ifndef _APP_THEME_H
#define _APP_THEME_H

#define DEFAULT_THEME_FILE_PATH "resources/theme/default_theme.txt"
#define NUM_ENTRIES_EXPECTED_IN_DEFAULT_THEME (40 + ThemeColor_Count*3) //this should be an overestimate

// X(EnumName)
#define Theme_XList(X)                       \
	X(OptionListBack)                        \
	X(ScrollGutter)                          \
	X(ScrollBar)                             \
	X(ScrollBarHover)                        \
	X(ScrollBarGrab)                         \
	X(TooltipBack)                           \
	X(TooltipBorder)                         \
	X(TooltipText)                           \
	X(TopmostBorder)                         \
	X(TopbarBack)                            \
	X(TopbarBorder)                          \
	X(TopbarPathText)                        \
	X(DropdownBack)                          \
	X(DropdownBorder)                        \
	X(OptionBackOff)                         \
	X(OptionBackOn)                          \
	X(OptionBackOnHovered)                   \
	X(OptionBackOffHovered)                  \
	X(OptionBackTurningOff)                  \
	X(OptionBackTurningOn)                   \
	X(OptionBorderOff)                       \
	X(OptionBorderOn)                        \
	X(OptionBorderOffHovered)                \
	X(OptionBorderOnHovered)                 \
	X(OptionBorderTurningOff)                \
	X(OptionBorderTurningOn)                 \
	X(OptionNameTextOff)                     \
	X(OptionNameTextOn)                      \
	X(OptionNameTextOffHovered)              \
	X(OptionNameTextOnHovered)               \
	X(OptionNameTextTurningOff)              \
	X(OptionNameTextTurningOn)               \
	X(OptionValueTextOff)                    \
	X(OptionValueTextOn)                     \
	X(OptionValueTextOffHovered)             \
	X(OptionValueTextOnHovered)              \
	X(OptionValueTextTurningOff)             \
	X(OptionValueTextTurningOn)              \
	X(TopbarBtnBack)                         \
	X(TopbarBtnBackHover)                    \
	X(TopbarBtnBackOpen)                     \
	X(TopbarBtnBorder)                       \
	X(TopbarBtnBorderHover)                  \
	X(TopbarBtnBorderOpen)                   \
	X(TopbarBtnText)                         \
	X(TopbarBtnTextHover)                    \
	X(TopbarBtnTextOpen)                     \
	X(DropdownBtnBack)                       \
	X(DropdownBtnBackHover)                  \
	X(DropdownBtnBackPressed)                \
	X(DropdownBtnBackOpen)                   \
	X(DropdownBtnBackDisabled)               \
	X(DropdownBtnBorder)                     \
	X(DropdownBtnBorderHover)                \
	X(DropdownBtnBorderPressed)              \
	X(DropdownBtnBorderOpen)                 \
	X(DropdownBtnBorderDisabled)             \
	X(DropdownBtnText)                       \
	X(DropdownBtnTextHover)                  \
	X(DropdownBtnTextPressed)                \
	X(DropdownBtnTextOpen)                   \
	X(DropdownBtnTextDisabled)               \
	X(DropdownBtnIcon)                       \
	X(DropdownBtnIconHover)                  \
	X(DropdownBtnIconPressed)                \
	X(DropdownBtnIconOpen)                   \
	X(DropdownBtnIconDisabled)               \
	X(HotkeyBack)                            \
	X(HotkeyBackHover)                       \
	X(HotkeyBackPressed)                     \
	X(HotkeyBackDisabled)                    \
	X(HotkeyBorder)                          \
	X(HotkeyBorderHover)                     \
	X(HotkeyBorderPressed)                   \
	X(HotkeyBorderDisabled)                  \
	X(HotkeyText)                            \
	X(HotkeyTextHover)                       \
	X(HotkeyTextPressed)                     \
	X(HotkeyTextDisabled)                    \
	X(FileTabDivider)                        \
	X(FileTabBack)                           \
	X(FileTabBackHover)                      \
	X(FileTabBackOpen)                       \
	X(FileTabBorder)                         \
	X(FileTabBorderHover)                    \
	X(FileTabBorderOpen)                     \
	X(FileTabText)                           \
	X(FileTabTextHover)                      \
	X(FileTabTextOpen)                       \
	X(ConfirmDialogDarken)                   \
	X(ConfirmDialogBack)                     \
	X(ConfirmDialogBorder)                   \
	X(ConfirmDialogText)                     \
	X(ConfirmDialogPositiveBtnBack)          \
	X(ConfirmDialogPositiveBtnBackHover)     \
	X(ConfirmDialogPositiveBtnBackPressed)   \
	X(ConfirmDialogPositiveBtnBorder)        \
	X(ConfirmDialogPositiveBtnBorderHover)   \
	X(ConfirmDialogPositiveBtnBorderPressed) \
	X(ConfirmDialogPositiveBtnText)          \
	X(ConfirmDialogPositiveBtnTextHover)     \
	X(ConfirmDialogPositiveBtnTextPressed)   \
	X(ConfirmDialogNegativeBtnBack)          \
	X(ConfirmDialogNegativeBtnBackHover)     \
	X(ConfirmDialogNegativeBtnBackPressed)   \
	X(ConfirmDialogNegativeBtnBorder)        \
	X(ConfirmDialogNegativeBtnBorderHover)   \
	X(ConfirmDialogNegativeBtnBorderPressed) \
	X(ConfirmDialogNegativeBtnText)          \
	X(ConfirmDialogNegativeBtnTextHover)     \
	X(ConfirmDialogNegativeBtnTextPressed)   \
	X(ConfirmDialogNeutralBtnBack)           \
	X(ConfirmDialogNeutralBtnBackHover)      \
	X(ConfirmDialogNeutralBtnBackPressed)    \
	X(ConfirmDialogNeutralBtnBorder)         \
	X(ConfirmDialogNeutralBtnBorderHover)    \
	X(ConfirmDialogNeutralBtnBorderPressed)  \
	X(ConfirmDialogNeutralBtnText)           \
	X(ConfirmDialogNeutralBtnTextHover)      \
	X(ConfirmDialogNeutralBtnTextPressed)    \
	X(NotificationDebugBack)                 \
	X(NotificationDebugBorder)               \
	X(NotificationDebugText)                 \
	X(NotificationDebugIcon)                 \
	X(NotificationRegularBack)               \
	X(NotificationRegularBorder)             \
	X(NotificationRegularText)               \
	X(NotificationRegularIcon)               \
	X(NotificationInfoBack)                  \
	X(NotificationInfoBorder)                \
	X(NotificationInfoText)                  \
	X(NotificationInfoIcon)                  \
	X(NotificationNotifyBack)                \
	X(NotificationNotifyBorder)              \
	X(NotificationNotifyText)                \
	X(NotificationNotifyIcon)                \
	X(NotificationOtherBack)                 \
	X(NotificationOtherBorder)               \
	X(NotificationOtherText)                 \
	X(NotificationOtherIcon)                 \
	X(NotificationWarningBack)               \
	X(NotificationWarningBorder)             \
	X(NotificationWarningText)               \
	X(NotificationWarningIcon)               \
	X(NotificationErrorBack)                 \
	X(NotificationErrorBorder)               \
	X(NotificationErrorText)                 \
	X(NotificationErrorIcon)

typedef enum ThemeColor ThemeColor;
enum ThemeColor
{
	ThemeColor_None = 0,
	
	#define X(EnumName) ThemeColor_##EnumName,
	Theme_XList(X)
	#undef X
	
	ThemeColor_Count,
};
const char* GetThemeColorStr(ThemeColor enumValue)
{
	switch (enumValue)
	{
		case ThemeColor_None: return "None";
		
		#define X(EnumName) case ThemeColor_##EnumName: return #EnumName;
		Theme_XList(X)
		#undef X
		
		default: return UNKNOWN_STR;
	}
}
bool TryParseThemeColor(Str8 themeColorStr, ThemeColor* colorOut)
{
	if (StrAnyCaseEquals(themeColorStr, StrLit("None"))) { SetOptionalOutPntr(colorOut, ThemeColor_None); return true; }
	
	#define X(EnumName) else if (StrAnyCaseEquals(themeColorStr, StrLit(#EnumName))) { SetOptionalOutPntr(colorOut, ThemeColor_##EnumName); return true; }
	Theme_XList(X)
	#undef X
	
	else { return false; }
}

typedef enum ThemeMode ThemeMode;
enum ThemeMode
{
	ThemeMode_None = 0,
	ThemeMode_Dark,
	ThemeMode_Light,
	ThemeMode_Debug,
	ThemeMode_Count,
};
const char* GetThemeModeStr(ThemeMode enumValue)
{
	switch (enumValue)
	{
		case ThemeMode_None:  return "None";
		case ThemeMode_Dark:  return "Dark";
		case ThemeMode_Light: return "Light";
		case ThemeMode_Debug: return "Debug";
		default: return UNKNOWN_STR;
	}
}
bool TryParseThemeMode(Str8 modeStr, ThemeMode* themeModeOut)
{
	if      (StrAnyCaseEquals(modeStr, StrLit("None")))  { SetOptionalOutPntr(themeModeOut, ThemeMode_None);  return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Dark")))  { SetOptionalOutPntr(themeModeOut, ThemeMode_Dark);  return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Light"))) { SetOptionalOutPntr(themeModeOut, ThemeMode_Light); return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Debug"))) { SetOptionalOutPntr(themeModeOut, ThemeMode_Debug); return true; }
	else                                                      { return false; }
}

typedef plex BakedTheme BakedTheme;
plex BakedTheme
{
	Color32 colors[ThemeColor_Count];
};

typedef plex ThemeDefEntry ThemeDefEntry;
plex ThemeDefEntry
{
	ThemeMode mode;
	Str8 key;
	bool isStrValue;
	bool isResolved;
	bool isReferenced;
	Color32 valueColor;
	Str8 valueStr;
};

typedef plex ThemeDefinition ThemeDefinition;
plex ThemeDefinition
{
	Arena* arena;
	VarArray entries; //ThemeDefEntry
};

ThemeDefEntry* FindThemeDefEntry(ThemeDefinition* theme, ThemeMode mode, Str8 key)
{
	VarArrayLoop(&theme->entries, eIndex)
	{
		VarArrayLoopGet(ThemeDefEntry, entry, &theme->entries, eIndex);
		if ((entry->mode == ThemeMode_None || entry->mode == mode) &&
			StrExactEquals(entry->key, key))
		{
			return entry;
		}
	}
	return nullptr;
}

// Returns false if an entry already existed and is getting overridden
inline bool AddThemeDefEntryColor(ThemeDefinition* theme, ThemeMode mode, Str8 key, Color32 color)
{
	NotNull(theme);
	NotNull(theme->arena);
	ThemeDefEntry* existingEntry = FindThemeDefEntry(theme, mode, key);
	if (existingEntry != nullptr)
	{
		if (existingEntry->isStrValue && CanArenaFree(theme->arena)) { FreeStr8(theme->arena, &existingEntry->valueStr); }
		existingEntry->isStrValue = false;
		existingEntry->valueColor = color;
		return false;
	}
	ThemeDefEntry* newEntry = VarArrayAdd(ThemeDefEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->isStrValue = false;
	newEntry->valueColor = color;
	return true;
}
inline bool AddThemeDefEntryStr(ThemeDefinition* theme, ThemeMode mode, Str8 key, Str8 value)
{
	NotNull(theme);
	NotNull(theme->arena);
	ThemeDefEntry* existingEntry = FindThemeDefEntry(theme, mode, key);
	if (existingEntry != nullptr)
	{
		if (existingEntry->isStrValue && CanArenaFree(theme->arena)) { FreeStr8(theme->arena, &existingEntry->valueStr); }
		existingEntry->isStrValue = true;
		existingEntry->valueStr = AllocStr8(theme->arena, value);
		return false;
	}
	ThemeDefEntry* newEntry = VarArrayAdd(ThemeDefEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->isStrValue = true;
	newEntry->valueStr = AllocStr8(theme->arena, value);
	return true;
}

#endif //  _APP_THEME_H
