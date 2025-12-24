/*
File:   app_theme_funcs.c
Author: Taylor Robbins
Date:   12\24\2025
Description:
	** This file is included before app_theme.c but after app_theme.h so we have access to types like ThemeDefinition but not the API
	** This file mostly serves as a home for CallThemeDefFunc which relies on the function declarations near the top and also does
	** argument count and type checking using the results of ParseThemeDefFuncArgStr which is cached in a global array ThemeDefFuncArgInfos (filled in InitThemeDefFuncArgInfos)
	** NOTE: Since the ThemeDefFuncArgInfo contains slices of string literals it needs to be re-initialized if the app dll is reloaded
*/

#define ThemeDefFunc_DEFINITION(functionName) Color32 functionName(const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
typedef ThemeDefFunc_DEFINITION(ThemeDefFunc_f);

ThemeDefFunc_DEFINITION(ThemeFunc_Mix);
ThemeDefFunc_DEFINITION(ThemeFunc_Transparent);
ThemeDefFunc_DEFINITION(ThemeFunc_Darken);
ThemeDefFunc_DEFINITION(ThemeFunc_Lighten);

typedef plex ThemeDefFuncArgInfo ThemeDefFuncArgInfo;
plex ThemeDefFuncArgInfo
{
	uxx argCount;
	ThemeDefFuncArgType argType[THEME_DEF_FUNC_MAX_ARGS];
	Str8 argName[THEME_DEF_FUNC_MAX_ARGS];
};

static ThemeDefFuncArgInfo ThemeDefFuncArgInfos[ThemeDefFunc_Count] = ZEROED;

ThemeDefFuncArgInfo ParseThemeDefFuncArgStr(const char* argsStrNt)
{
	ThemeDefFuncArgInfo result = ZEROED;
	
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
			Assert(result.argCount < THEME_DEF_FUNC_MAX_ARGS);
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
			Assert(argType != ThemeDefFuncArgType_None);
			
			result.argType[result.argCount] = argType;
			result.argName[result.argCount] = nameStr;
			result.argCount++;
			
			prevCommaIndex = cIndex+1;
		}
	}
	
	return result;
}

ThemeDefFuncArgInfo GetThemeDefFuncArgInfo(ThemeDefFunc enumValue)
{
	switch (enumValue)
	{
		#define X(FuncName, CFuncName, NumArgs, ArgStr) case ThemeDefFunc_##FuncName: return ParseThemeDefFuncArgStr(ArgStr);
		ThemeDefFunc_XList(X)
		#undef X
		
		default: return NEW_STRUCT(ThemeDefFuncArgInfo)ZEROED;
	}
}

void InitThemeDefFuncArgInfos()
{
	for (uxx fIndex = 1; fIndex < ThemeDefFunc_Count; fIndex++)
	{
		ThemeDefFuncArgInfos[fIndex] = GetThemeDefFuncArgInfo((ThemeDefFunc)fIndex);
	}
}

Color32 CallThemeDefFunc(ThemeDefFunc enumValue, const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
{
	switch (enumValue)
	{
		#define X(FuncName, CFuncName, NumArgs, ArgStr)                                                               \
		case ThemeDefFunc_##FuncName:                                                                                 \
		{                                                                                                             \
			DebugAssert(numArgs == NumArgs);                                                                          \
			DebugAssert(NumArgs == ThemeDefFuncArgInfos[ThemeDefFunc_##FuncName].argCount);                           \
			DebugAssert(argValues != nullptr || NumArgs == 0);                                                        \
			for (uxx aIndex = 0; aIndex < NumArgs; aIndex++)                                                          \
			{                                                                                                         \
				DebugAssert(argValues[aIndex].type == ThemeDefFuncArgInfos[ThemeDefFunc_##FuncName].argType[aIndex]); \
			}                                                                                                         \
			return CFuncName(themeDef, numArgs, argValues);                                                           \
		}
		
		ThemeDefFunc_XList(X)
		
		#undef X
		
		default: AssertMsg(false, "Invalid enum value passed to CallThemeDefFunc!"); return Transparent;
	}
}