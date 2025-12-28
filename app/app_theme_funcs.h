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
uxx GetThemeDefFuncArgCount(ThemeDefFunc enumValue)
{
	switch (enumValue)
	{
		#define X(FuncName, CFuncName, NumArgs, ArgStr) case ThemeDefFunc_##FuncName: return NumArgs;
		ThemeDefFunc_XList(X)
		#undef X
		
		default: return 0;
	}
}

typedef plex ThemeDefFuncArgInfo ThemeDefFuncArgInfo;
plex ThemeDefFuncArgInfo
{
	uxx argCount;
	ThemeDefFuncArgType argType[THEME_DEF_FUNC_MAX_ARGS];
	Str8 argName[THEME_DEF_FUNC_MAX_ARGS];
};

Result TryParseThemeDefFuncArgStr(const char* argsStrNt, ThemeDefFuncArgInfo* infoOut)
{
	ClearPointer(infoOut);
	
	Str8 argsStr = TrimWhitespace(MakeStr8Nt(argsStrNt));
	if (argsStr.length >= 1 && argsStr.chars[0] == '(') { argsStr.chars++; argsStr.length--; }
	if (argsStr.length >= 1 && argsStr.chars[argsStr.length-1] == ')') { argsStr.length--; }
	argsStr = TrimWhitespace(argsStr);
	
	uxx prevCommaIndex = 0;
	for (uxx cIndex = 0; cIndex <= argsStr.length; cIndex++)
	{
		char nextChar = (cIndex < argsStr.length) ? argsStr.chars[cIndex] : '\0';
		if (nextChar == ',' || nextChar == '\0')
		{
			if (infoOut->argCount >= THEME_DEF_FUNC_MAX_ARGS) { return Result_TooMany; }
			Str8 argStr = TrimWhitespace(StrSlice(argsStr, prevCommaIndex, cIndex));
			Assert(argStr.length > 1);
			
			uxx whitespaceIndex = FindNextWhitespaceInStrEx(argStr, 0, false);
			Assert(whitespaceIndex > 0 && whitespaceIndex < argsStr.length);
			Str8 typeStr = StrSlice(argStr, 0, whitespaceIndex);
			Str8 nameStr = TrimWhitespace(StrSliceFrom(argStr, whitespaceIndex+1));
			
			ThemeDefFuncArgType argType = ThemeDefFuncArgType_None;
			for (uxx tIndex = 1; tIndex < ThemeDefFuncArgType_Count; tIndex++)
			{
				ThemeDefFuncArgType enumValue = (ThemeDefFuncArgType)tIndex;
				Str8 enumValueName = MakeStr8Nt(GetThemeDefFuncArgTypeStr(enumValue));
				if (StrAnyCaseEquals(typeStr, enumValueName)) { argType = enumValue; break; }
			}
			if (argType == ThemeDefFuncArgType_None) { return Result_InvalidType; }
			
			if (!IsValidIdentifier(nameStr.length, nameStr.chars, false, false, false)) { return Result_InvalidCharacter; }
			
			infoOut->argType[infoOut->argCount] = argType;
			infoOut->argName[infoOut->argCount] = nameStr;
			infoOut->argCount++;
			
			prevCommaIndex = cIndex+1;
		}
	}
	
	return Result_Success;
}

ThemeDefFuncArgInfo GetThemeDefFuncArgInfo(ThemeDefFunc enumValue)
{
	ThemeDefFuncArgInfo result = ZEROED;
	switch (enumValue)
	{
		#define X(FuncName, CFuncName, NumArgs, ArgStr) case ThemeDefFunc_##FuncName: { Result parseResult = TryParseThemeDefFuncArgStr(ArgStr, &result); Assert(parseResult == Result_Success); } break;
		ThemeDefFunc_XList(X)
		#undef X
		
		default: Assert(false); return result;
	}
	return result;
}

#endif //  _APP_THEME_FUNCS_H
