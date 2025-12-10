/*
File:   app_theme.h
Author: Taylor Robbins
Date:   12\10\2025
*/

#ifndef _APP_THEME_H
#define _APP_THEME_H

#define DarkThemeColor_BackgroundBlack    MakeColor(25, 27, 28, 255)    //FF191B1C
#define DarkThemeColor_BackgroundDarkGray MakeColor(31, 34, 35, 255)    //FF1F2223
#define DarkThemeColor_BackgroundGray     MakeColor(39, 42, 43, 255)    //FF272A2B
#define DarkThemeColor_TextWhite          MakeColor(221, 222, 224, 255) //FFDDDEE0
#define DarkThemeColor_TextLightGray      MakeColor(175, 177, 179, 255) //FFAFB1B3
#define DarkThemeColor_TextGray           MakeColor(107, 112, 120, 255) //FF6B7078
#define DarkThemeColor_Outline            MakeColor(52, 58, 59, 255)    //FF343A3B
#define DarkThemeColor_Hovered            MakeColor(16, 60, 76, 255)    //FF103C4C
#define DarkThemeColor_Selected           MakeColor(0, 121, 166, 255)   //FF0079A6
#define DarkThemeColor_Error              MakeColor(255, 102, 102, 255) //FFFF6666

#define LightThemeColor_BackgroundBlack    MakeColor(255, 255, 255, 255) //FFFFFFFF
#define LightThemeColor_BackgroundDarkGray MakeColor(248, 248, 248, 255) //FFF8F8F8
#define LightThemeColor_BackgroundGray     MakeColor(235, 235, 235, 255) //FFEBEBEB
#define LightThemeColor_TextWhite          MakeColor(62, 63, 64, 255)    //FF3E3F40
#define LightThemeColor_TextLightGray      MakeColor(96, 98, 99, 255)    //FF606263
#define LightThemeColor_TextGray           MakeColor(144, 146, 147, 255) //FF909293
#define LightThemeColor_Outline            MakeColor(218, 218, 219, 255) //FFDADADB
#define LightThemeColor_Hovered            MakeColor(153, 222, 248, 255) //FF99DEF8
#define LightThemeColor_Selected           MakeColor(0, 173, 238, 255)   //FF00ADEE
#define LightThemeColor_Error              MakeColor(255, 102, 102, 255) //FFFF6666

#define DebugThemeColor_BackgroundBlack    PalRed    //FFF92672
#define DebugThemeColor_BackgroundDarkGray PalPink   //FFED4E95
#define DebugThemeColor_BackgroundGray     PalOrange //FFFD971F
#define DebugThemeColor_TextWhite          PalHoney  //FFFDC459
#define DebugThemeColor_TextLightGray      PalBanana //FFEEE64F
#define DebugThemeColor_TextGray           PalGreen  //FFA6E22E
#define DebugThemeColor_Outline            PalGrass  //FF92BB41
#define DebugThemeColor_Hovered            PalBlue   //FF669BEF
#define DebugThemeColor_Selected           PalSky    //FF66D9EF
#define DebugThemeColor_Error              PalPurple //FFAE81FF

typedef enum ThemeColor ThemeColor;
enum ThemeColor
{
	ThemeColor_None = 0,
	ThemeColor_BackgroundBlack,
	ThemeColor_BackgroundDarkGray,
	ThemeColor_BackgroundGray,
	ThemeColor_TextWhite,
	ThemeColor_TextLightGray,
	ThemeColor_TextGray,
	ThemeColor_Outline,
	ThemeColor_Hovered,
	ThemeColor_Selected,
	ThemeColor_Error,
	ThemeColor_Count,
};
const char* GetThemeColorStr(ThemeColor enumValue)
{
	switch (enumValue)
	{
		case ThemeColor_None:               return "None";
		case ThemeColor_BackgroundBlack:    return "BackgroundBlack";
		case ThemeColor_BackgroundDarkGray: return "BackgroundDarkGray";
		case ThemeColor_BackgroundGray:     return "BackgroundGray";
		case ThemeColor_TextWhite:          return "TextWhite";
		case ThemeColor_TextLightGray:      return "TextLightGray";
		case ThemeColor_TextGray:           return "TextGray";
		case ThemeColor_Outline:            return "Outline";
		case ThemeColor_Hovered:            return "Hovered";
		case ThemeColor_Selected:           return "Selected";
		case ThemeColor_Error:              return "Error";
		default: return UNKNOWN_STR;
	}
}

typedef plex Theme Theme;
plex Theme
{
	bool overridden[ThemeColor_Count];
	Color32 colors[ThemeColor_Count];
};

Color32 GetColorFromThemeOrDefault(Theme* defaultTheme, Theme* theme, ThemeColor themeColor)
{
	NotNull(theme);
	if (defaultTheme != nullptr && !theme->overridden[themeColor]) { return defaultTheme->colors[themeColor]; }
	else { return theme->colors[themeColor]; }
}
inline void SetThemeColor(Theme* theme, ThemeColor themeColor, Color32 colorValue)
{
	theme->overridden[themeColor] = true;
	theme->colors[themeColor] = colorValue;
}

typedef enum PresetTheme PresetTheme;
enum PresetTheme
{
	PresetTheme_None = 0,
	PresetTheme_Dark,
	PresetTheme_Light,
	PresetTheme_Debug,
	PresetTheme_Count,
};
const char* GetPresetThemeStr(PresetTheme enumValue)
{
	switch (enumValue)
	{
		case PresetTheme_None:  return "None";
		case PresetTheme_Dark:  return "Dark";
		case PresetTheme_Light: return "Light";
		case PresetTheme_Debug: return "Debug";
		default: return UNKNOWN_STR;
	}
}

#define InitDarkThemePreset(themePntr) do                                                         \
{                                                                                                 \
	SetThemeColor((themePntr), ThemeColor_BackgroundBlack,    DarkThemeColor_BackgroundBlack);    \
	SetThemeColor((themePntr), ThemeColor_BackgroundDarkGray, DarkThemeColor_BackgroundDarkGray); \
	SetThemeColor((themePntr), ThemeColor_BackgroundGray,     DarkThemeColor_BackgroundGray);     \
	SetThemeColor((themePntr), ThemeColor_TextWhite,          DarkThemeColor_TextWhite);          \
	SetThemeColor((themePntr), ThemeColor_TextLightGray,      DarkThemeColor_TextLightGray);      \
	SetThemeColor((themePntr), ThemeColor_TextGray,           DarkThemeColor_TextGray);           \
	SetThemeColor((themePntr), ThemeColor_Outline,            DarkThemeColor_Outline);            \
	SetThemeColor((themePntr), ThemeColor_Hovered,            DarkThemeColor_Hovered);            \
	SetThemeColor((themePntr), ThemeColor_Selected,           DarkThemeColor_Selected);           \
	SetThemeColor((themePntr), ThemeColor_Error,              DarkThemeColor_Error);              \
} while(0)

#define InitLightThemePreset(themePntr) do                                                         \
{                                                                                                  \
	SetThemeColor((themePntr), ThemeColor_BackgroundBlack,    LightThemeColor_BackgroundBlack);    \
	SetThemeColor((themePntr), ThemeColor_BackgroundDarkGray, LightThemeColor_BackgroundDarkGray); \
	SetThemeColor((themePntr), ThemeColor_BackgroundGray,     LightThemeColor_BackgroundGray);     \
	SetThemeColor((themePntr), ThemeColor_TextWhite,          LightThemeColor_TextWhite);          \
	SetThemeColor((themePntr), ThemeColor_TextLightGray,      LightThemeColor_TextLightGray);      \
	SetThemeColor((themePntr), ThemeColor_TextGray,           LightThemeColor_TextGray);           \
	SetThemeColor((themePntr), ThemeColor_Outline,            LightThemeColor_Outline);            \
	SetThemeColor((themePntr), ThemeColor_Hovered,            LightThemeColor_Hovered);            \
	SetThemeColor((themePntr), ThemeColor_Selected,           LightThemeColor_Selected);           \
	SetThemeColor((themePntr), ThemeColor_Error,              LightThemeColor_Error);              \
} while(0)

#define InitDebugThemePreset(themePntr) do                                                         \
{                                                                                                  \
	SetThemeColor((themePntr), ThemeColor_BackgroundBlack,    DebugThemeColor_BackgroundBlack);    \
	SetThemeColor((themePntr), ThemeColor_BackgroundDarkGray, DebugThemeColor_BackgroundDarkGray); \
	SetThemeColor((themePntr), ThemeColor_BackgroundGray,     DebugThemeColor_BackgroundGray);     \
	SetThemeColor((themePntr), ThemeColor_TextWhite,          DebugThemeColor_TextWhite);          \
	SetThemeColor((themePntr), ThemeColor_TextLightGray,      DebugThemeColor_TextLightGray);      \
	SetThemeColor((themePntr), ThemeColor_TextGray,           DebugThemeColor_TextGray);           \
	SetThemeColor((themePntr), ThemeColor_Outline,            DebugThemeColor_Outline);            \
	SetThemeColor((themePntr), ThemeColor_Hovered,            DebugThemeColor_Hovered);            \
	SetThemeColor((themePntr), ThemeColor_Selected,           DebugThemeColor_Selected);           \
	SetThemeColor((themePntr), ThemeColor_Error,              DebugThemeColor_Error);              \
} while(0)

#endif //  _APP_THEME_H
