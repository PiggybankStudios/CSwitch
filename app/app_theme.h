/*
File:   app_theme.h
Author: Taylor Robbins
Date:   12\10\2025
*/

#ifndef _APP_THEME_H
#define _APP_THEME_H

#define DEFAULT_THEME_FILE_PATH "resources/text/default_theme.txt"
#define NUM_ENTRIES_EXPECTED_IN_DEFAULT_THEME (40 + ThemeColor_Count*3) //this should be an overestimate

// The Theme_XList macro needs to be able to define all possible states for a color entry, thus we have this flag-style enum that maps to entries in the ThemeState enum
typedef enum ThemeStateFlag ThemeStateFlag;
enum ThemeStateFlag
{
	ThemeStateFlag_None        = 0x00,
	ThemeStateFlag_Clickable   = 0x01, //Hovered and Pressed
	ThemeStateFlag_Selectable  = 0x02,
	ThemeStateFlag_Disableable = 0x04,
	ThemeStateFlag_Openable    = 0x08,
};

//For brevity sake in the XList we #define these here and #undef them below after all the code that uses Theme_XList
#define S_None        ThemeStateFlag_None
#define S_Clickable   ThemeStateFlag_Clickable
#define S_Selectable  ThemeStateFlag_Selectable
#define S_Disableable ThemeStateFlag_Disableable
#define S_Openable    ThemeStateFlag_Openable

// X(EnumName, StateFlags)
#define Theme_XList(X)                                                             \
	X(OptionListBack,                        S_None)                               \
	X(ScrollGutter,                          S_None)                               \
	X(ScrollBar,                             S_Clickable)                          \
	X(TooltipBack,                           S_None)                               \
	X(TooltipBorder,                         S_None)                               \
	X(TooltipText,                           S_None)                               \
	X(TopmostBorder,                         S_None)                               \
	X(TopbarBack,                            S_None)                               \
	X(TopbarBorder,                          S_None)                               \
	X(TopbarPathText,                        S_None)                               \
	X(DropdownBack,                          S_None)                               \
	X(DropdownBorder,                        S_None)                               \
	X(OptionOnBack,                          S_Clickable|S_Selectable)             \
	X(OptionOnBorder,                        S_Clickable|S_Selectable)             \
	X(OptionOnNameText,                      S_Clickable|S_Selectable)             \
	X(OptionOnValueText,                     S_Clickable|S_Selectable)             \
	X(OptionOffBack,                         S_Clickable|S_Selectable)             \
	X(OptionOffBorder,                       S_Clickable|S_Selectable)             \
	X(OptionOffNameText,                     S_Clickable|S_Selectable)             \
	X(OptionOffValueText,                    S_Clickable|S_Selectable)             \
	X(TopbarBtnBack,                         S_Clickable|S_Openable)               \
	X(TopbarBtnBorder,                       S_Clickable|S_Openable)               \
	X(TopbarBtnText,                         S_Clickable|S_Openable)               \
	X(DropdownBtnBack,                       S_Clickable|S_Disableable|S_Openable) \
	X(DropdownBtnBorder,                     S_Clickable|S_Disableable|S_Openable) \
	X(DropdownBtnText,                       S_Clickable|S_Disableable|S_Openable) \
	X(DropdownBtnIcon,                       S_Clickable|S_Disableable|S_Openable) \
	X(HotkeyBack,                            S_Clickable|S_Disableable)            \
	X(HotkeyBorder,                          S_Clickable|S_Disableable)            \
	X(HotkeyText,                            S_Clickable|S_Disableable)            \
	X(FileTabDivider,                        S_None)                               \
	X(FileTabBack,                           S_Clickable|S_Openable)               \
	X(FileTabBorder,                         S_Clickable|S_Openable)               \
	X(FileTabText,                           S_Clickable|S_Openable)               \
	X(ConfirmDialogDarken,                   S_None)                               \
	X(ConfirmDialogBack,                     S_None)                               \
	X(ConfirmDialogBorder,                   S_None)                               \
	X(ConfirmDialogText,                     S_None)                               \
	X(ConfirmDialogPositiveBtnBack,          S_Clickable)                          \
	X(ConfirmDialogPositiveBtnBorder,        S_Clickable)                          \
	X(ConfirmDialogPositiveBtnText,          S_Clickable)                          \
	X(ConfirmDialogNegativeBtnBack,          S_Clickable)                          \
	X(ConfirmDialogNegativeBtnBorder,        S_Clickable)                          \
	X(ConfirmDialogNegativeBtnText,          S_Clickable)                          \
	X(ConfirmDialogNeutralBtnBack,           S_Clickable)                          \
	X(ConfirmDialogNeutralBtnBorder,         S_Clickable)                          \
	X(ConfirmDialogNeutralBtnText,           S_Clickable)                          \
	X(NotificationDebugBack,                 S_None)                               \
	X(NotificationDebugBorder,               S_None)                               \
	X(NotificationDebugText,                 S_None)                               \
	X(NotificationDebugIcon,                 S_None)                               \
	X(NotificationRegularBack,               S_None)                               \
	X(NotificationRegularBorder,             S_None)                               \
	X(NotificationRegularText,               S_None)                               \
	X(NotificationRegularIcon,               S_None)                               \
	X(NotificationInfoBack,                  S_None)                               \
	X(NotificationInfoBorder,                S_None)                               \
	X(NotificationInfoText,                  S_None)                               \
	X(NotificationInfoIcon,                  S_None)                               \
	X(NotificationNotifyBack,                S_None)                               \
	X(NotificationNotifyBorder,              S_None)                               \
	X(NotificationNotifyText,                S_None)                               \
	X(NotificationNotifyIcon,                S_None)                               \
	X(NotificationOtherBack,                 S_None)                               \
	X(NotificationOtherBorder,               S_None)                               \
	X(NotificationOtherText,                 S_None)                               \
	X(NotificationOtherIcon,                 S_None)                               \
	X(NotificationWarningBack,               S_None)                               \
	X(NotificationWarningBorder,             S_None)                               \
	X(NotificationWarningText,               S_None)                               \
	X(NotificationWarningIcon,               S_None)                               \
	X(NotificationErrorBack,                 S_None)                               \
	X(NotificationErrorBorder,               S_None)                               \
	X(NotificationErrorText,                 S_None)                               \
	X(NotificationErrorIcon,                 S_None)

typedef enum ThemeColor ThemeColor;
enum ThemeColor
{
	ThemeColor_None = 0,
	
	#define X(EnumName, StateFlags) ThemeColor_##EnumName,
	Theme_XList(X)
	#undef X
	
	ThemeColor_Count,
};
const char* GetThemeColorStr(ThemeColor enumValue)
{
	switch (enumValue)
	{
		case ThemeColor_None: return "None";
		
		#define X(EnumName, StateFlags) case ThemeColor_##EnumName: return #EnumName;
		Theme_XList(X)
		#undef X
		
		default: return UNKNOWN_STR;
	}
}
bool TryParseThemeColor(Str8 themeColorStr, ThemeColor* colorOut)
{
	if (StrAnyCaseEquals(themeColorStr, StrLit("None"))) { SetOptionalOutPntr(colorOut, ThemeColor_None); return true; }
	
	#define X(EnumName, StateFlags) else if (StrAnyCaseEquals(themeColorStr, StrLit(#EnumName))) { SetOptionalOutPntr(colorOut, ThemeColor_##EnumName); return true; }
	Theme_XList(X)
	#undef X
	
	else { return false; }
}
u8 GetThemeColorStateFlags(ThemeColor themeColor)
{
	switch (themeColor)
	{
		#define X(EnumName, StateFlags) case ThemeColor_##EnumName: return (StateFlags);
		Theme_XList(X)
		#undef X
		
		default: return ThemeStateFlag_None;
	}
}

#undef S_None
#undef S_Clickable
#undef S_Selectable
#undef S_Disableable
#undef S_Openable
#undef Theme_XList

// Every theme can be in a "Light" or "Dark" mode and should have different colors chosen for both
// (the "Debug" mode is a special mode that simply fills all entries with a unique color using GetPredefPalColorByIndex)
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

// ThemeState contains possible bits of state that can affect the color of a UI element (esp. mouse interactable elements like buttons)
// Not all UI elements experience all states (for example some buttons cannot be "disabled", and non-dropdown elements can't be "open")
// Generally blocks of states are in an ascending priority order. A "pressed" button ignores "hovered" and "open" state
typedef enum ThemeState ThemeState;
enum ThemeState
{
	ThemeState_Any     = 0,
	ThemeState_Default = 1,
	ThemeState_Disabled,
	ThemeState_Pressed,
	ThemeState_Selected,
	ThemeState_Hovered,
	ThemeState_Open,
	ThemeState_Count,
};
const char* GetThemeStateStr(ThemeState enumValue)
{
	switch (enumValue)
	{
		case ThemeState_Any:      return "Any";
		case ThemeState_Default:  return "Default";
		case ThemeState_Disabled: return "Disabled";
		case ThemeState_Pressed:  return "Pressed";
		case ThemeState_Selected: return "Selected";
		case ThemeState_Hovered:  return "Hovered";
		case ThemeState_Open:     return "Open";
		default: return UNKNOWN_STR;
	}
}
bool TryParseThemeState(Str8 modeStr, ThemeState* themeStateOut)
{
	if      (StrAnyCaseEquals(modeStr, StrLit("Any")))      { SetOptionalOutPntr(themeStateOut, ThemeState_Any);      return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Default")))  { SetOptionalOutPntr(themeStateOut, ThemeState_Default);  return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Disabled"))) { SetOptionalOutPntr(themeStateOut, ThemeState_Disabled); return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Pressed")))  { SetOptionalOutPntr(themeStateOut, ThemeState_Pressed);  return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Hovered")))  { SetOptionalOutPntr(themeStateOut, ThemeState_Hovered);  return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Selected"))) { SetOptionalOutPntr(themeStateOut, ThemeState_Selected); return true; }
	else if (StrAnyCaseEquals(modeStr, StrLit("Open")))     { SetOptionalOutPntr(themeStateOut, ThemeState_Open);     return true; }
	else                                                    { return false; }
}
bool IsThemeStatePossibleFromFlags(u8 flags, ThemeState state)
{
	if (state == ThemeState_Default || state == ThemeState_Any) { return true; }
	if (IsFlagSet(flags, ThemeStateFlag_Clickable) && (state == ThemeState_Hovered || state == ThemeState_Pressed)) { return true; }
	if (IsFlagSet(flags, ThemeStateFlag_Selectable) && state == ThemeState_Selected) { return true; }
	if (IsFlagSet(flags, ThemeStateFlag_Disableable) && state == ThemeState_Disabled) { return true; }
	if (IsFlagSet(flags, ThemeStateFlag_Openable) && state == ThemeState_Open) { return true; }
	return false;
}
u8 GetThemeStateFlagForState(ThemeState state)
{
	switch (state)
	{
		case ThemeState_Hovered:  return ThemeStateFlag_Clickable;
		case ThemeState_Pressed:  return ThemeStateFlag_Clickable;
		case ThemeState_Selected: return ThemeStateFlag_Selectable;
		case ThemeState_Disabled: return ThemeStateFlag_Disableable;
		case ThemeState_Open:     return ThemeStateFlag_Openable;
		default: return ThemeStateFlag_None;
	}
}

typedef plex BakedThemeEntry BakedThemeEntry;
plex BakedThemeEntry
{
	Color32 colors[ThemeState_Count];
};

//A ThemeDefinition can be "baked" into this structure which is what is actually used by the UI code
typedef plex BakedTheme BakedTheme;
plex BakedTheme
{
	BakedThemeEntry entries[ThemeColor_Count];
};

typedef enum ThemeDefEntryType ThemeDefEntryType;
enum ThemeDefEntryType
{
	ThemeDefEntryType_None = 0,
	ThemeDefEntryType_Color, //Name: RRGGBB
	ThemeDefEntryType_Reference, //Name: OtherName
	ThemeDefEntryType_Function, //Name: Func(Arg1, Arg2)
	ThemeDefEntryType_Count,
};

typedef plex ThemeDefEntry ThemeDefEntry;
plex ThemeDefEntry
{
	ThemeMode mode;
	ThemeState state;
	Str8 key;
	
	bool isResolved;
	bool isReferenced;
	
	ThemeDefEntryType type;
	
	//only filled if (type == ThemeDefEntryType_Color || isResolved)
	Color32 color;
	
	//only filled if (type == ThemeDefEntryType_Reference)
	Str8 referenceKey;
	
	//only filled if (type == ThemeDefEntryType_Function)
	ThemeDefFunc function;
	uxx functionArgCount;
	ThemeDefFuncArgValue functionArgValues[THEME_DEF_FUNC_MAX_ARGS];
};

// A theme "definition" contains a loose set of key-value pairs where the value can be an identifier of another entry
// These identifiers are resolved in BakeTheme. key-value pairs that have a value that is parsable as a color are handled during definition parsing
typedef plex ThemeDefinition ThemeDefinition;
plex ThemeDefinition
{
	Arena* arena;
	VarArray entries; //ThemeDefEntry
};

void FreeThemeDefEntryValue(ThemeDefinition* theme, ThemeDefEntry* entry)
{
	NotNull(theme);
	NotNull(theme->arena);
	NotNull(entry);
	if (CanArenaFree(theme->arena))
	{
		if (entry->type == ThemeDefEntryType_Reference)
		{
			FreeStr8(theme->arena, &entry->referenceKey);
		}
		if (entry->type == ThemeDefEntryType_Function)
		{
			for (uxx aIndex = 0; aIndex < entry->functionArgCount; aIndex++)
			{
				if (entry->functionArgValues[aIndex].type == ThemeDefFuncArgType_Identifier)
				{
					FreeStr8(theme->arena, &entry->functionArgValues[aIndex].valueIdentifier);
				}
			}
		}
	}
}
void FreeThemeDefEntry(ThemeDefinition* theme, ThemeDefEntry* entry)
{
	NotNull(theme);
	NotNull(theme->arena);
	if (CanArenaFree(theme->arena))
	{
		FreeStr8(theme->arena, &entry->key);
		FreeThemeDefEntryValue(theme, entry);
	}
	ClearPointer(entry);
}

ThemeDefEntry* FindThemeDefEntry(ThemeDefinition* theme, ThemeMode mode, bool allowModeNone, ThemeState state, bool allowStateAny, Str8 key)
{
	//TODO: We should probably sort the entry list and lookup entries using binary search!
	//      In my testing so far this doesn't cause performance problems yet
	//      It takes about 8ms to load default theme (which has 335 entries) on startup which is a minority of our load time
	//TODO: We need to prioritize mode matches over state matches!
	ThemeDefEntry* result = nullptr;
	VarArrayLoop(&theme->entries, eIndex)
	{
		VarArrayLoopGet(ThemeDefEntry, entry, &theme->entries, eIndex);
		if (StrExactEquals(entry->key, key))
		{
			if (entry->mode == mode && entry->state == state)
			{
				result = entry;
				break; //breaking early prioritizes exact matches over None/Any matches
			}
			else if (allowModeNone || allowStateAny)
			{
				if ((entry->mode == mode || (allowModeNone && entry->mode == ThemeMode_None)) &&
					(entry->state == state || (allowStateAny && entry->state == ThemeState_Any)))
				{
					result = entry;
					//don't break the loop early
				}
			}
		}
	}
	return result;
}

// Returns false if an entry already existed and is getting overridden
bool AddThemeDefEntryColor(ThemeDefinition* theme, ThemeMode mode, ThemeState state, Str8 key, Color32 color)
{
	NotNull(theme);
	NotNull(theme->arena);
	ThemeDefEntry* existingEntry = FindThemeDefEntry(theme, mode, false, state, false, key);
	if (existingEntry != nullptr)
	{
		FreeThemeDefEntryValue(theme, existingEntry);
		existingEntry->type = ThemeDefEntryType_Color;
		existingEntry->color = color;
		return false;
	}
	ThemeDefEntry* newEntry = VarArrayAdd(ThemeDefEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->state = state;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->type = ThemeDefEntryType_Color;
	newEntry->color = color;
	return true;
}
// Returns false if an entry already existed and is getting overridden
bool AddThemeDefEntryReference(ThemeDefinition* theme, ThemeMode mode, ThemeState state, Str8 key, Str8 referenceStr)
{
	NotNull(theme);
	NotNull(theme->arena);
	ThemeDefEntry* existingEntry = FindThemeDefEntry(theme, mode, false, state, false, key);
	if (existingEntry != nullptr)
	{
		FreeThemeDefEntryValue(theme, existingEntry);
		existingEntry->type = ThemeDefEntryType_Reference;
		existingEntry->referenceKey = AllocStr8(theme->arena, referenceStr);
		return false;
	}
	ThemeDefEntry* newEntry = VarArrayAdd(ThemeDefEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->state = state;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->type = ThemeDefEntryType_Reference;
	newEntry->referenceKey = AllocStr8(theme->arena, referenceStr);
	return true;
}
// Returns false if an entry already existed and is getting overridden
bool AddThemeDefEntryFunction(ThemeDefinition* theme, ThemeMode mode, ThemeState state, Str8 key, ThemeDefFunc function, uxx numArgs, const ThemeDefFuncArgValue* argValues)
{
	NotNull(theme);
	NotNull(theme->arena);
	ThemeDefEntry* existingEntry = FindThemeDefEntry(theme, mode, false, state, false, key);
	if (existingEntry != nullptr)
	{
		FreeThemeDefEntryValue(theme, existingEntry);
		existingEntry->type = ThemeDefEntryType_Function;
		existingEntry->function = function;
		existingEntry->functionArgCount = numArgs;
		for (uxx aIndex = 0; aIndex < numArgs; aIndex++)
		{
			MyMemCopy(&existingEntry->functionArgValues[aIndex], &argValues[aIndex], sizeof(ThemeDefFuncArgValue));
			if (argValues[aIndex].type == ThemeDefFuncArgType_Identifier)
			{
				existingEntry->functionArgValues[aIndex].valueIdentifier = AllocStr8(theme->arena, existingEntry->functionArgValues[aIndex].valueIdentifier);
			}
		}
		return false;
	}
	ThemeDefEntry* newEntry = VarArrayAdd(ThemeDefEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->state = state;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->type = ThemeDefEntryType_Function;
	newEntry->function = function;
	newEntry->functionArgCount = numArgs;
	for (uxx aIndex = 0; aIndex < numArgs; aIndex++)
	{
		MyMemCopy(&newEntry->functionArgValues[aIndex], &argValues[aIndex], sizeof(ThemeDefFuncArgValue));
		if (argValues[aIndex].type == ThemeDefFuncArgType_Identifier)
		{
			newEntry->functionArgValues[aIndex].valueIdentifier = AllocStr8(theme->arena, newEntry->functionArgValues[aIndex].valueIdentifier);
		}
	}
	return true;
}

bool AddThemeDefEntryPntr(ThemeDefinition* theme, const ThemeDefEntry* entryPntr)
{
	if (entryPntr->type == ThemeDefEntryType_Color)
	{
		return AddThemeDefEntryColor(theme, entryPntr->mode, entryPntr->state, entryPntr->key, entryPntr->color);
	}
	else if (entryPntr->type == ThemeDefEntryType_Reference)
	{
		return AddThemeDefEntryReference(theme, entryPntr->mode, entryPntr->state, entryPntr->key, entryPntr->referenceKey);
	}
	else if (entryPntr->type == ThemeDefEntryType_Function)
	{
		return AddThemeDefEntryFunction(theme, entryPntr->mode, entryPntr->state, entryPntr->key, entryPntr->function, entryPntr->functionArgCount, &entryPntr->functionArgValues[0]);
	}
	else { Assert(false); return false; }
}

#endif //  _APP_THEME_H
