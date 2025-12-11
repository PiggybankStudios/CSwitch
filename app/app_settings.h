/*
File:   app_settings.h
Author: Taylor Robbins
Date:   12\11\2025
Description:
	** "App Settings" are options that the user can change (usually through some overt action in the UI) that are
	** saved to disk in the AppData folder so that the values can be restored when the application is closed and reopened
	** The settings file is written every time a setting changes so generally these are not high-frequency options
	** TODO: Currently, whichever instance of CSwitch changes a setting last has the final say on all settings for future instances (settings are not auto-reloaded when the file changes on disk)
	** 
*/

//TODO: We should only save settings that have been changed by the user, so that the default values could be updated in the future and users that have explicitly chosen a setting will not be affected while new users or ones that haven't configured the setting will be affected
//TODO: Because we use TextParser and KeyValuePairs, string type settings cannot include new-line characters whitespace will be trimmed from beginning and end

#ifndef _APP_SETTINGS_H
#define _APP_SETTINGS_H

// NOTE: This X-Macro allows us to generate the struct and functions below
// X(type, cType, nameNt, cName, defaultValue)
#define Settings_XList(X)                                                    \
	X(Bool, bool, "SmallButtons", smallButtons, false)                       \
	X(Bool, bool, "ClipNamesLeft", clipNamesLeft, false)                     \
	X(Bool, bool, "SmoothScrollingDisabled", smoothScrollingDisabled, false) \
	X(Bool, bool, "OptionTooltipsDisabled", optionTooltipsDisabled, false)   \
	X(String, Str8, "Theme", theme, StrLit("Dark"))                          \
	X(String, Str8, "UserThemePath", userThemePath, StrLit(""))

typedef enum AppSettingType AppSettingType;
enum AppSettingType
{
	AppSettingType_None = 0,
	AppSettingType_String,
	AppSettingType_Bool,
	AppSettingType_U32,
	AppSettingType_U64,
	AppSettingType_I32,
	AppSettingType_I64,
	AppSettingType_R32,
	AppSettingType_Count,
};
const char* GetAppSettingTypeStr(AppSettingType enumValue)
{
	switch (enumValue)
	{
		case AppSettingType_None:   return "None";
		case AppSettingType_String: return "String";
		case AppSettingType_Bool:   return "Bool";
		case AppSettingType_U32:    return "U32";
		case AppSettingType_U64:    return "U64";
		case AppSettingType_I32:    return "I32";
		case AppSettingType_I64:    return "I64";
		case AppSettingType_R32:    return "R32";
		default: return UNKNOWN_STR;
	}
}

typedef plex AppSettings AppSettings;
plex AppSettings
{
	Arena* arena;
	uxx count;
	
	#define X(type, cType, nameNt, cName, defaultValue) cType cName;
	Settings_XList(X)
	#undef X
};

inline AppSettingType GetAppSettingTypeAtIndex(uxx settingIndex)
{
	const AppSettingType typeArray[] = {
		#define X(type, cType, nameNt, cName, defaultValue) AppSettingType_##type,
		Settings_XList(X)
		#undef X
	};
	if (settingIndex >= ArrayCount(typeArray)) { return AppSettingType_None; }
	return typeArray[settingIndex];
}
inline const char* GetAppSettingNameAtIndex(uxx settingIndex)
{
	const char* nameArray[] = {
		#define X(type, cType, nameNt, cName, defaultValue) nameNt,
		Settings_XList(X)
		#undef X
	};
	if (settingIndex >= ArrayCount(nameArray)) { return nullptr; }
	return nameArray[settingIndex];
}
inline void* GetAppSettingPntrAtIndex(AppSettings* settings, uxx settingIndex, uxx* sizeOut)
{
	const u32 memberOffsets[] = {
		#define X(type, cType, nameNt, cName, defaultValue) STRUCT_VAR_OFFSET(AppSettings, cName),
		Settings_XList(X)
		#undef X
	};
	const u32 memberSizes[] = {
		#define X(type, cType, nameNt, cName, defaultValue) STRUCT_VAR_SIZE(AppSettings, cName),
		Settings_XList(X)
		#undef X
	};
	if (settingIndex >= ArrayCount(memberOffsets)) { return nullptr; }
	SetOptionalOutPntr(sizeOut, memberSizes[settingIndex]);
	return ((u8*)settings) + memberOffsets[settingIndex];
}
inline uxx GetAppSettingIndexByPntr(const AppSettings* settings, const void* settingPntr)
{
	for (uxx sIndex = 0; sIndex < settings->count; sIndex++)
	{
		if (GetAppSettingPntrAtIndex((AppSettings*)settings, sIndex, nullptr) == settingPntr) { return sIndex; }
	}
	return UINTXX_MAX;
}

inline Str8* GetAppSettingPntrStr8(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_String);
	return (Str8*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline bool* GetAppSettingPntrBool(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_Bool);
	return (bool*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline u32* GetAppSettingPntrU32(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_U32);
	return (u32*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline u64* GetAppSettingPntrU64(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_U64);
	return (u64*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline i32* GetAppSettingPntrI32(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_I32);
	return (i32*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline i64* GetAppSettingPntrI64(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_I64);
	return (i64*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}
inline r32* GetAppSettingPntrR32(AppSettings* settings, uxx settingIndex)
{
	NotNull(settings);
	Assert(GetAppSettingTypeAtIndex(settingIndex) == AppSettingType_R32);
	return (r32*)GetAppSettingPntrAtIndex(settings, settingIndex, nullptr);
}

inline Str8 GetAppSettingStr8(const AppSettings* settings, uxx settingIndex)
{
	Str8* pntr = GetAppSettingPntrStr8((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : Str8_Empty;
}
inline bool GetAppSettingBool(const AppSettings* settings, uxx settingIndex)
{
	bool* pntr = GetAppSettingPntrBool((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : false;
}
inline u32 GetAppSettingU32(const AppSettings* settings, uxx settingIndex)
{
	u32* pntr = GetAppSettingPntrU32((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : 0;
}
inline u64 GetAppSettingU64(const AppSettings* settings, uxx settingIndex)
{
	u64* pntr = GetAppSettingPntrU64((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : 0;
}
inline i32 GetAppSettingI32(const AppSettings* settings, uxx settingIndex)
{
	i32* pntr = GetAppSettingPntrI32((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : 0;
}
inline i64 GetAppSettingI64(const AppSettings* settings, uxx settingIndex)
{
	i64* pntr = GetAppSettingPntrI64((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : 0;
}
inline r32 GetAppSettingR32(const AppSettings* settings, uxx settingIndex)
{
	r32* pntr = GetAppSettingPntrR32((AppSettings*)settings, settingIndex);
	return (pntr != nullptr) ? *pntr : 0;
}

inline void SetAppSettingStr8(AppSettings* settings, uxx settingIndex, Str8 value)
{
	Str8* pntr = GetAppSettingPntrStr8(settings, settingIndex);
	NotNull(pntr);
	NotNull(settings->arena);
	if (StrExactEquals(*pntr, value)) { return; }
	if (!IsEmptyStr(*pntr)) { FreeStr8(settings->arena, pntr); }
	*pntr = AllocStr8(settings->arena, value);
}
inline void SetAppSettingStr8Pntr(AppSettings* settings, Str8* settingPntr, Str8 value)
{
	SetAppSettingStr8(settings, GetAppSettingIndexByPntr(settings, settingPntr), value);
}
inline void SetAppSettingBool(AppSettings* settings, uxx settingIndex, bool value)
{
	bool* pntr = GetAppSettingPntrBool(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}
inline void SetAppSettingU32(AppSettings* settings, uxx settingIndex, u32 value)
{
	u32* pntr = GetAppSettingPntrU32(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}
inline void SetAppSettingU64(AppSettings* settings, uxx settingIndex, u64 value)
{
	u64* pntr = GetAppSettingPntrU64(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}
inline void SetAppSettingI32(AppSettings* settings, uxx settingIndex, i32 value)
{
	i32* pntr = GetAppSettingPntrI32(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}
inline void SetAppSettingI64(AppSettings* settings, uxx settingIndex, i64 value)
{
	i64* pntr = GetAppSettingPntrI64(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}
inline void SetAppSettingR32(AppSettings* settings, uxx settingIndex, r32 value)
{
	r32* pntr = GetAppSettingPntrR32(settings, settingIndex);
	NotNull(pntr);
	*pntr = value;
}

void FreeAppSettings(AppSettings* settings)
{
	NotNull(settings);
	if (settings->arena != nullptr)
	{
		#define X(type, cType, nameNt, cName, defaultValue) if (AppSettingType_##type == AppSettingType_String) { FreeStr8(settings->arena, (Str8*)&settings->cName); }
		Settings_XList(X)
		#undef X
	}
	ClearPointer(settings);
}
void InitAppSettings(Arena* arena, AppSettings* settingsOut)
{
	NotNull(settingsOut);
	ClearPointer(settingsOut);
	settingsOut->arena = arena;
	settingsOut->count = 0;
	#define X(type, cType, nameNt, cName, defaultValue) settingsOut->cName = defaultValue; settingsOut->count++;
	Settings_XList(X)
	#undef X
	for (uxx sIndex = 0; sIndex < settingsOut->count; sIndex++)
	{
		if (GetAppSettingTypeAtIndex(sIndex) == AppSettingType_String)
		{
			Str8* strPntr = GetAppSettingPntrStr8(settingsOut, sIndex);
			*strPntr = AllocStr8(settingsOut->arena, *strPntr);
		}
	}
}

Result TryParseAppSettingsFile(Str8 fileContents, AppSettings* settingsOut)
{
	NotNull(settingsOut);
	NotNull(settingsOut->arena);
	
	if (IsEmptyStr(fileContents)) { return Result_EmptyFile; }
	uxx numSettingsParsed = 0;
	
	TextParser parser = MakeTextParser(fileContents);
	ParsingToken token;
	while (TextParserGetToken(&parser, &token))
	{
		switch (token.type)
		{
			case ParsingTokenType_Comment: /* ignore comments */ break;
			
			case ParsingTokenType_KeyValuePair:
			{
				bool parsedSettingName = false;
				uxx settingIndex = UINTXX_MAX;
				Str8 settingName = Str8_Empty;
				AppSettingType settingType = AppSettingType_None;
				
				for (uxx sIndex = 0; sIndex < settingsOut->count; sIndex++)
				{
					Str8 loopSettingName = MakeStr8Nt(GetAppSettingNameAtIndex(sIndex));
					if (StrAnyCaseEquals(loopSettingName, token.key))
					{
						parsedSettingName = true;
						settingIndex = sIndex;
						settingName = loopSettingName;
						settingType = GetAppSettingTypeAtIndex(sIndex);
						break;
					}
				}
				
				if (parsedSettingName)
				{
					switch (settingType)
					{
						case AppSettingType_String:
						{
							//TODO: Should we do quotes removal and string un-escaping?
							SetAppSettingStr8(settingsOut, settingIndex, token.value);
							numSettingsParsed++;
						} break;
						
						case AppSettingType_Bool:
						{
							bool boolValue = false;
							if (TryParseBool(token.value, &boolValue, nullptr))
							{
								SetAppSettingBool(settingsOut, settingIndex, boolValue);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as bool: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						case AppSettingType_U32:
						{
							u32 u32Value = false;
							if (TryParseU32(token.value, &u32Value, nullptr))
							{
								SetAppSettingU32(settingsOut, settingIndex, u32Value);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as u32: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						case AppSettingType_U64:
						{
							u64 u64Value = false;
							if (TryParseU64(token.value, &u64Value, nullptr))
							{
								SetAppSettingU64(settingsOut, settingIndex, u64Value);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as u64: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						case AppSettingType_I32:
						{
							i32 i32Value = false;
							if (TryParseI32(token.value, &i32Value, nullptr))
							{
								SetAppSettingI32(settingsOut, settingIndex, i32Value);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as i32: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						case AppSettingType_I64:
						{
							i64 i64Value = false;
							if (TryParseI64(token.value, &i64Value, nullptr))
							{
								SetAppSettingI64(settingsOut, settingIndex, i64Value);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as i64: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						case AppSettingType_R32:
						{
							r32 r32Value = false;
							if (TryParseR32(token.value, &r32Value, nullptr))
							{
								SetAppSettingR32(settingsOut, settingIndex, r32Value);
								numSettingsParsed++;
							}
							else
							{
								PrintLine_E("Failed to parse %.*s in settings file as r32: \"%.*s\"", StrPrint(settingName), StrPrint(token.value));
							}
						} break;
						
						default:
						{
							DebugAssertMsg(false, "Unhandled AppSettingType in TryParseAppSettingsFile!");
						} break;
					}
				}
				else
				{
					PrintLine_W("Unknown setting on line %llu of settings file: \"%.*s\"", parser.lineParser.lineIndex, token.key);
				}
			} break;
			
			default:
			{
				PrintLine_W("Unhandled token on line %llu of settings file: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.str));
			} break;
		}
	}
	
	if (numSettingsParsed == 0) { return Result_NoMatch; }
	return Result_Success;
}
Result TryLoadAppSettingsFrom(FilePath filePath, AppSettings* settingsOut)
{
	NotNull(settingsOut);
	NotNull(settingsOut->arena);
	ScratchBegin1(scratch, settingsOut->arena);
	Str8 fileContents = Str8_Empty;
	if (!OsReadTextFile(filePath, scratch, &fileContents)) { ScratchEnd(scratch); return Result_FailedToReadFile; }
	Result result = TryParseAppSettingsFile(fileContents, settingsOut);
	ScratchEnd(scratch);
	return result;
}

Str8 SerializeAppSettings(Arena* arena, const AppSettings* settings)
{
	TwoPassStr8Loop(result, arena, false)
	{
		TwoPassPrint(&result, "// WARNING: This file is automatically overwritten when CSwitch is running. Please close CSwitch before making any hand edits\n\n");
		for (uxx sIndex = 0; sIndex < settings->count; sIndex++)
		{
			AppSettingType settingType = GetAppSettingTypeAtIndex(sIndex);
			switch (settingType)
			{
				case AppSettingType_String: Str8 strValue = GetAppSettingStr8(settings, sIndex); TwoPassPrint(&result, "%s: %.*s\n", GetAppSettingNameAtIndex(sIndex), StrPrint(strValue)); break;
				case AppSettingType_Bool: TwoPassPrint(&result, "%s: %s\n",   GetAppSettingNameAtIndex(sIndex), GetAppSettingBool(settings, sIndex) ? "True" : "False"); break;
				case AppSettingType_U32:  TwoPassPrint(&result, "%s: %u\n",   GetAppSettingNameAtIndex(sIndex), GetAppSettingU32(settings, sIndex)); break;
				case AppSettingType_U64:  TwoPassPrint(&result, "%s: %llu\n", GetAppSettingNameAtIndex(sIndex), GetAppSettingU64(settings, sIndex)); break;
				case AppSettingType_I32:  TwoPassPrint(&result, "%s: %d\n",   GetAppSettingNameAtIndex(sIndex), GetAppSettingI32(settings, sIndex)); break;
				case AppSettingType_I64:  TwoPassPrint(&result, "%s: %lld\n", GetAppSettingNameAtIndex(sIndex), GetAppSettingI64(settings, sIndex)); break;
				case AppSettingType_R32:  TwoPassPrint(&result, "%s: %f\n",   GetAppSettingNameAtIndex(sIndex), GetAppSettingR32(settings, sIndex)); break;
				default: DebugAssertMsg(false, "Unhandled AppSettingType in SerializeAppSettings"); break;
			}
		}
		TwoPassStr8LoopEnd(&result);
	}
	return result.str;
}
bool TrySaveAppSettingsTo(const AppSettings* settings, FilePath filePath)
{
	ScratchBegin(scratch);
	Str8 serializedSettings = SerializeAppSettings(scratch, settings);
	bool writeFileSuccess = OsWriteTextFile(filePath, serializedSettings);
	ScratchEnd(scratch);
	return writeFileSuccess;
}

#endif //  _APP_SETTINGS_H
