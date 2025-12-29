/*
File:   app_theme_funcs_implementations.c
Author: Taylor Robbins
Date:   12\24\2025
Description:
	** This file is included after app_theme.c so we have full access to the ThemeDefinition API if needed (to resolve identifiers and such)
*/

// +==============================+
// |        ThemeFunc_Mix         |
// +==============================+
// Color32 ThemeFunc_Mix(const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Mix)
{
	UNUSED(themeDef);
	UNUSED(mode);
	UNUSED(numArgs);
	return Color32Lerp(argValues[0].valueColor, argValues[1].valueColor, argValues[2].valueR32);
}

// +==============================+
// |    ThemeFunc_Transparent     |
// +==============================+
// Color32 ThemeFunc_Transparent(const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Transparent)
{
	UNUSED(themeDef);
	UNUSED(mode);
	UNUSED(numArgs);
	if (argValues[1].type == ThemeDefFuncArgType_U8)
	{
		return ColorWithAlphaU8(argValues[0].valueColor, argValues[1].valueU8);
	}
	else if (argValues[1].type == ThemeDefFuncArgType_R32)
	{
		return ColorWithAlpha(argValues[0].valueColor, argValues[1].valueR32);
	}
	else { Assert(false); return argValues[0].valueColor; }
}

// +==============================+
// |       ThemeFunc_Darken       |
// +==============================+
// Color32 ThemeFunc_Darken(const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Darken)
{
	UNUSED(themeDef);
	UNUSED(mode);
	UNUSED(numArgs);
	// return ColorDarken(argValues[0].valueColor, argValues[1].valueR32);
	DebugAssert(false); //TODO: Remove me once the function above is implemented
	return argValues[0].valueColor;
}

// +==============================+
// |      ThemeFunc_Lighten       |
// +==============================+
// Color32 ThemeFunc_Lighten(const ThemeDefinition* themeDef, ThemeMode mode, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Lighten)
{
	UNUSED(themeDef);
	UNUSED(mode);
	UNUSED(numArgs);
	// return ColorLighten(argValues[0].valueColor, argValues[1].valueR32);
	DebugAssert(false); //TODO: Remove me once the function above is implemented
	return argValues[0].valueColor;
}
