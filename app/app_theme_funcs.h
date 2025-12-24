/*
File:   app_theme_funcs.h
Author: Taylor Robbins
Date:   12\24\2025
Description:
	** Inside theme definitions we have a small set of functions that can be called with a variable number of arguments
	** This a somewhat small part of the theme definition but involves a little complexity in the implementation so it's
	** split out into it's own files to make the main app_theme.h\.c a little smaller
*/

#ifndef _APP_THEME_FUNCS_H
#define _APP_THEME_FUNCS_H

#define THEME_DEF_FUNC_MAX_ARGS  3

//NOTE: Don't forget to add the function declartion in app_theme_funcs.c and implementation in app_theme_func_implementations.c using ThemeDefFunc_DEFINITION calling convention
// X(FuncName, CFuncName, NumArgs, ArgStr)
#define ThemeDefFunc_XList(X)                                                                  \
	X(Mix,         ThemeFunc_Mix,         3, "(Color ColorStart, Color ColorEnd, r32 Amount)") \
	X(Transparent, ThemeFunc_Transparent, 2, "(Color Color, r32 Alpha)")                       \
	X(Darken,      ThemeFunc_Darken,      2, "(Color Color, r32 Amount)")                      \
	X(Lighten,     ThemeFunc_Lighten,     2, "(Color Color, r32 Amount)")

typedef enum ThemeDefFunc ThemeDefFunc;
enum ThemeDefFunc
{
	ThemeDefFunc_None = 0,
	
	#define X(FuncName, CFuncName, NumArgs, ArgStr) ThemeDefFunc_##FuncName,
	ThemeDefFunc_XList(X)
	#undef X
	
	ThemeDefFunc_Count,
};

typedef enum ThemeDefFuncArgType ThemeDefFuncArgType;
enum ThemeDefFuncArgType
{
	ThemeDefFuncArgType_None = 0,
	ThemeDefFuncArgType_U8,
	ThemeDefFuncArgType_I32,
	ThemeDefFuncArgType_R32,
	ThemeDefFuncArgType_Color,
	ThemeDefFuncArgType_Identifier,
	ThemeDefFuncArgType_Count,
};
const char* GetThemeDefFuncArgTypeStr(ThemeDefFuncArgType enumValue)
{
	switch (enumValue)
	{
		case ThemeDefFuncArgType_None:       return "None";
		case ThemeDefFuncArgType_U8:         return "U8";
		case ThemeDefFuncArgType_I32:        return "I32";
		case ThemeDefFuncArgType_R32:        return "R32";
		case ThemeDefFuncArgType_Color:      return "Color";
		case ThemeDefFuncArgType_Identifier: return "Identifier";
		default: return UNKNOWN_STR;
	}
}

typedef plex ThemeDefFuncArgValue ThemeDefFuncArgValue;
plex ThemeDefFuncArgValue
{
	ThemeDefFuncArgType type;
	car
	{
		u8 valueU8;
		i32 valueI32;
		r32 valueR32;
		Color32 valueColor;
		Str8 valueIdentifier;
	};
};

const char* GetThemeDefFuncStr(ThemeDefFunc enumValue)
{
	switch (enumValue)
	{
		case ThemeDefFunc_None: return "None";
		
		#define X(FuncName, CFuncName, NumArgs, ArgStr) case ThemeDefFunc_##FuncName: return #FuncName;
		ThemeDefFunc_XList(X)
		#undef X
		
		default: return UNKNOWN_STR;
	}
}

#endif //  _APP_THEME_FUNCS_H
