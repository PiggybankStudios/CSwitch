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

#define ThemeDefFunc_DEFINITION(functionName) Color32 functionName(const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
typedef ThemeDefFunc_DEFINITION(ThemeDefFunc_f);

ThemeDefFunc_DEFINITION(ThemeFunc_Mix);
ThemeDefFunc_DEFINITION(ThemeFunc_Transparent);
ThemeDefFunc_DEFINITION(ThemeFunc_Darken);
ThemeDefFunc_DEFINITION(ThemeFunc_Lighten);

static ThemeDefFuncArgInfo ThemeDefFuncArgInfos[ThemeDefFunc_Count] = ZEROED;

void InitThemeDefFuncArgInfos()
{
	for (uxx fIndex = 1; fIndex < ThemeDefFunc_Count; fIndex++)
	{
		ThemeDefFuncArgInfos[fIndex] = GetThemeDefFuncArgInfo((ThemeDefFunc)fIndex);
	}
}

Color32 CallThemeDefFunc(ThemeDefFunc enumValue, const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
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
			return CFuncName(themeDef, mode, numArgs, argValues);                                                     \
		}
		
		ThemeDefFunc_XList(X)
		
		#undef X
		
		default: AssertMsg(false, "Invalid enum value passed to CallThemeDefFunc!"); return Transparent;
	}
}