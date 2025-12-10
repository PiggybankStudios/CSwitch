/*
File:   app_theme.c
Author: Taylor Robbins
Date:   12\10\2025
Description: 
	** Holds functions related to interacting with Theme structures which holds a set of colors and override booleans for each slot
	** Theme structures are used to store colors that we use for UI elements. It's not a complex structure but it allows us to let the user pick from multiple themes (primarily "light" and "dark" themes)
*/

#define GetThemeColor(themeColorSuffix) GetColorFromThemeOrDefault(&app->themePresets[app->currentThemePreset], &app->themeOverrides, ThemeColor_##themeColorSuffix)

//TODO: Add serialization functions here to allow saving themes to disk

