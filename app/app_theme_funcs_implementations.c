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
// Color32 ThemeFunc_Mix(const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Mix)
{
	UNUSED(themeDef);
	UNUSED(numArgs);
	return Color32Lerp(argValues[0].valueColor, argValues[1].valueColor, argValues[2].valueR32);
}

// +==============================+
// |    ThemeFunc_Transparent     |
// +==============================+
// Color32 ThemeFunc_Transparent(const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Transparent)
{
	UNUSED(themeDef);
	UNUSED(numArgs);
	return ColorWithAlpha(argValues[0].valueColor, argValues[1].valueR32);
}

// +==============================+
// |       ThemeFunc_Darken       |
// +==============================+
// Color32 ThemeFunc_Darken(const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Darken)
{
	UNUSED(themeDef);
	UNUSED(numArgs);
	// return ColorDarken(argValues[0].valueColor, argValues[1].valueR32);
	return argValues[0].valueColor; //TODO: Remove me once the function above is implemented
}

// +==============================+
// |      ThemeFunc_Lighten       |
// +==============================+
// Color32 ThemeFunc_Lighten(const ThemeDefinition* themeDef, uxx numArgs, const ThemeDefFuncArgValue* argValues)
ThemeDefFunc_DEFINITION(ThemeFunc_Lighten)
{
	UNUSED(themeDef);
	UNUSED(numArgs);
	// return ColorLighten(argValues[0].valueColor, argValues[1].valueR32);
	return argValues[0].valueColor; //TODO: Remove me once the function above is implemented
}
