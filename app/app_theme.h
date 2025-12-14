/*
File:   app_theme.h
Author: Taylor Robbins
Date:   12\10\2025
*/

#ifndef _APP_THEME_H
#define _APP_THEME_H

#define ThemeVar_Transparent      MakeColor(0, 0, 0, 0) //00000000
#define ThemeVar_Darken25         MakeColor(0, 0, 0, 64) //40000000
#define ThemeVar_MonokaiDarkGray  MonokaiDarkGray
#define ThemeVar_MonokaiGray1     MonokaiGray1
#define ThemeVar_MonokaiGreen     MonokaiGreen
#define ThemeVar_MonokaiLightBlue MonokaiLightBlue
#define ThemeVar_MonokaiLightGray MonokaiLightGray
#define ThemeVar_MonokaiMagenta   MonokaiMagenta
#define ThemeVar_MonokaiOrange    MonokaiOrange
#define ThemeVar_MonokaiPurple    MonokaiPurple

#define DarkThemeVar_Black             MakeColorU32(0xFF191B1C)
#define DarkThemeVar_DarkGray          MakeColorU32(0xFF1F2223)
#define DarkThemeVar_Gray              MakeColorU32(0xFF272A2B)
#define DarkThemeVar_White             MakeColorU32(0xFFDDDEE0)
#define DarkThemeVar_LighterGray       MakeColorU32(0xFFAFB1B3)
#define DarkThemeVar_LightGray         MakeColorU32(0xFF6B7078)
#define DarkThemeVar_LightGray50       MakeColorU32(0x806B7078)
#define DarkThemeVar_DarkestBlue       MakeColorU32(0xFF343A3B)
#define DarkThemeVar_DarkBlue          MakeColorU32(0xFF103C4C)
#define DarkThemeVar_Blue              MakeColorU32(0xFF0079A6)
#define DarkThemeVar_Blue50            MakeColorU32(0x800079A6)
#define DarkThemeVar_Red               MakeColorU32(0xFFFF6666)
#define DarkThemeVar_Red50             MakeColorU32(0x80FF6666)
#define DarkThemeVar_DarkBlueToBlue1   MakeColorU32(0xFF085B79)
#define DarkThemeVar_DarkBlueToBlue2   MakeColorU32(0xFF046A90)

#define LightThemeVar_White            MakeColorU32(0xFFFFFFFF)
#define LightThemeVar_NearWhite        MakeColorU32(0xFFEBEBEB)
#define LightThemeVar_DarkGray         MakeColorU32(0xFF3E3F40)
#define LightThemeVar_Gray             MakeColorU32(0xFF606263)
#define LightThemeVar_LightGray        MakeColorU32(0xFF909293)
#define LightThemeVar_LightGray50      MakeColorU32(0x80909293)
#define LightThemeVar_BlueWhite        MakeColorU32(0xFF56A7D6)
#define LightThemeVar_LightBlue        MakeColorU32(0xFF99DEF8)
#define LightThemeVar_Blue             MakeColorU32(0xFF00ADEE)
#define LightThemeVar_Blue50           MakeColorU32(0x8000ADEE)
#define LightThemeVar_Red              MakeColorU32(0xFFFF6666)
#define LightThemeVar_Red50            MakeColorU32(0x80FF6666)
#define LightThemeVar_LightBlueToBlue1 MakeColorU32(0xFF4DC6F3)
#define LightThemeVar_LightBlueToBlue2 MakeColorU32(0xFF26B9F1)

// +--------------------------------------------------------------+
// |                      Dark Theme Colors                       |
// +--------------------------------------------------------------+
	#define DarkThemeColor_OptionListBack                        "Black"
	#define DarkThemeColor_ScrollGutter                          "Black"
	#define DarkThemeColor_ScrollBar                             "DarkestBlue"
	#define DarkThemeColor_ScrollBarHover                        "LightGray"
	#define DarkThemeColor_ScrollBarGrab                         "LightGray"
	#define DarkThemeColor_TooltipBack                           "White"
	#define DarkThemeColor_TooltipBorder                         "Transparent"
	#define DarkThemeColor_TooltipText                           "DarkGray"
	#define DarkThemeColor_TopmostBorder                         "Blue"
	#define DarkThemeColor_TopbarBack                            "Gray"
	#define DarkThemeColor_TopbarBorder                          "DarkestBlue"
	#define DarkThemeColor_TopbarPathText                        "LighterGray"
	#define DarkThemeColor_DropdownBack                          "Gray"
	#define DarkThemeColor_DropdownBorder                        "DarkestBlue"
	#define DarkThemeColor_OptionBackOff                         "Transparent"
	#define DarkThemeColor_OptionBackOn                          "Blue"
	#define DarkThemeColor_OptionBackOnHovered                   "DarkBlueToBlue2"
	#define DarkThemeColor_OptionBackOffHovered                  "DarkBlue"
	#define DarkThemeColor_OptionBackTurningOff                  "DarkBlueToBlue1"
	#define DarkThemeColor_OptionBackTurningOn                   "DarkBlueToBlue1"
	#define DarkThemeColor_OptionBorderOff                       "DarkestBlue"
	#define DarkThemeColor_OptionBorderOn                        "Blue"
	#define DarkThemeColor_OptionBorderOffHovered                "DarkBlueToBlue1"
	#define DarkThemeColor_OptionBorderOnHovered                 "Blue"
	#define DarkThemeColor_OptionBorderTurningOff                "DarkBlueToBlue1"
	#define DarkThemeColor_OptionBorderTurningOn                 "Blue"
	#define DarkThemeColor_OptionNameTextOff                     "LightGray"
	#define DarkThemeColor_OptionNameTextOn                      "White"
	#define DarkThemeColor_OptionNameTextOffHovered              "LightGray"
	#define DarkThemeColor_OptionNameTextOnHovered               "White"
	#define DarkThemeColor_OptionNameTextTurningOff              "LightGray"
	#define DarkThemeColor_OptionNameTextTurningOn               "White"
	#define DarkThemeColor_OptionValueTextOff                    "LightGray"
	#define DarkThemeColor_OptionValueTextOn                     "White"
	#define DarkThemeColor_OptionValueTextOffHovered             "LightGray"
	#define DarkThemeColor_OptionValueTextOnHovered              "White"
	#define DarkThemeColor_OptionValueTextTurningOff             "LightGray"
	#define DarkThemeColor_OptionValueTextTurningOn              "White"
	#define DarkThemeColor_TopbarBtnBack                         "Transparent"
	#define DarkThemeColor_TopbarBtnBackHover                    "DarkBlue"
	#define DarkThemeColor_TopbarBtnBackOpen                     "DarkBlue"
	#define DarkThemeColor_TopbarBtnBorder                       "Transparent"
	#define DarkThemeColor_TopbarBtnBorderHover                  "Blue"
	#define DarkThemeColor_TopbarBtnBorderOpen                   "Transparent"
	#define DarkThemeColor_TopbarBtnText                         "White"
	#define DarkThemeColor_TopbarBtnTextHover                    "White"
	#define DarkThemeColor_TopbarBtnTextOpen                     "White"
	#define DarkThemeColor_DropdownBtnBack                       "Transparent"
	#define DarkThemeColor_DropdownBtnBackHover                  "DarkBlue"
	#define DarkThemeColor_DropdownBtnBackPressed                "Blue"
	#define DarkThemeColor_DropdownBtnBackOpen                   "DarkBlue"
	#define DarkThemeColor_DropdownBtnBackDisabled               "Black"
	#define DarkThemeColor_DropdownBtnBorder                     "Transparent"
	#define DarkThemeColor_DropdownBtnBorderHover                "Blue"
	#define DarkThemeColor_DropdownBtnBorderPressed              "Transparent"
	#define DarkThemeColor_DropdownBtnBorderOpen                 "Transparent"
	#define DarkThemeColor_DropdownBtnBorderDisabled             "Transparent"
	#define DarkThemeColor_DropdownBtnText                       "White"
	#define DarkThemeColor_DropdownBtnTextHover                  "White"
	#define DarkThemeColor_DropdownBtnTextPressed                "White"
	#define DarkThemeColor_DropdownBtnTextOpen                   "White"
	#define DarkThemeColor_DropdownBtnTextDisabled               "LightGray"
	#define DarkThemeColor_DropdownBtnIcon                       "White"
	#define DarkThemeColor_DropdownBtnIconHover                  "White"
	#define DarkThemeColor_DropdownBtnIconPressed                "White"
	#define DarkThemeColor_DropdownBtnIconOpen                   "White"
	#define DarkThemeColor_DropdownBtnIconDisabled               "LightGray"
	#define DarkThemeColor_HotkeyBack                            "Transparent"
	#define DarkThemeColor_HotkeyBackHover                       "Transparent"
	#define DarkThemeColor_HotkeyBackPressed                     "Transparent"
	#define DarkThemeColor_HotkeyBackDisabled                    "Transparent"
	#define DarkThemeColor_HotkeyBorder                          "LightGray"
	#define DarkThemeColor_HotkeyBorderHover                     "LightGray"
	#define DarkThemeColor_HotkeyBorderPressed                   "LightGray"
	#define DarkThemeColor_HotkeyBorderDisabled                  "LightGray"
	#define DarkThemeColor_HotkeyText                            "LightGray"
	#define DarkThemeColor_HotkeyTextHover                       "LightGray"
	#define DarkThemeColor_HotkeyTextPressed                     "LightGray"
	#define DarkThemeColor_HotkeyTextDisabled                    "LightGray"
	#define DarkThemeColor_FileTabDivider                        "LightGray"
	#define DarkThemeColor_FileTabBack                           "Gray"
	#define DarkThemeColor_FileTabBackHover                      "DarkestBlue"
	#define DarkThemeColor_FileTabBackOpen                       "Black"
	#define DarkThemeColor_FileTabBorder                         "Transparent"
	#define DarkThemeColor_FileTabBorderHover                    "Blue"
	#define DarkThemeColor_FileTabBorderOpen                     "Transparent"
	#define DarkThemeColor_FileTabText                           "LighterGray"
	#define DarkThemeColor_FileTabTextHover                      "White"
	#define DarkThemeColor_FileTabTextOpen                       "White"
	#define DarkThemeColor_ConfirmDialogDarken                   "Darken25"
	#define DarkThemeColor_ConfirmDialogBack                     "Gray"
	#define DarkThemeColor_ConfirmDialogBorder                   "LighterGray"
	#define DarkThemeColor_ConfirmDialogText                     "White"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBack          "Transparent"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBackHover     "Blue50"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBackPressed   "Blue50"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorder        "Blue50"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorderHover   "Blue"
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorderPressed "Blue"
	#define DarkThemeColor_ConfirmDialogPositiveBtnText          "White"
	#define DarkThemeColor_ConfirmDialogPositiveBtnTextHover     "White"
	#define DarkThemeColor_ConfirmDialogPositiveBtnTextPressed   "White"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBack          "Transparent"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBackHover     "Red50"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBackPressed   "Red50"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorder        "Red50"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorderHover   "Red"
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorderPressed "Red"
	#define DarkThemeColor_ConfirmDialogNegativeBtnText          "White"
	#define DarkThemeColor_ConfirmDialogNegativeBtnTextHover     "White"
	#define DarkThemeColor_ConfirmDialogNegativeBtnTextPressed   "White"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBack           "Transparent"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBackHover      "LightGray50"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBackPressed    "LightGray50"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorder         "LightGray50"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorderHover    "LightGray"
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorderPressed  "LightGray"
	#define DarkThemeColor_ConfirmDialogNeutralBtnText           "White"
	#define DarkThemeColor_ConfirmDialogNeutralBtnTextHover      "White"
	#define DarkThemeColor_ConfirmDialogNeutralBtnTextPressed    "White"
	#define DarkThemeColor_NotificationDebugBack                 "MonokaiDarkGray"
	#define DarkThemeColor_NotificationDebugBorder               "MonokaiLightGray"
	#define DarkThemeColor_NotificationDebugText                 "White"
	#define DarkThemeColor_NotificationDebugIcon                 "MonokaiGray1"
	#define DarkThemeColor_NotificationRegularBack               "MonokaiDarkGray"
	#define DarkThemeColor_NotificationRegularBorder             "MonokaiLightGray"
	#define DarkThemeColor_NotificationRegularText               "White"
	#define DarkThemeColor_NotificationRegularIcon               "White"
	#define DarkThemeColor_NotificationInfoBack                  "MonokaiDarkGray"
	#define DarkThemeColor_NotificationInfoBorder                "MonokaiLightGray"
	#define DarkThemeColor_NotificationInfoText                  "White"
	#define DarkThemeColor_NotificationInfoIcon                  "MonokaiGreen"
	#define DarkThemeColor_NotificationNotifyBack                "MonokaiDarkGray"
	#define DarkThemeColor_NotificationNotifyBorder              "MonokaiLightGray"
	#define DarkThemeColor_NotificationNotifyText                "White"
	#define DarkThemeColor_NotificationNotifyIcon                "MonokaiPurple"
	#define DarkThemeColor_NotificationOtherBack                 "MonokaiDarkGray"
	#define DarkThemeColor_NotificationOtherBorder               "MonokaiLightGray"
	#define DarkThemeColor_NotificationOtherText                 "White"
	#define DarkThemeColor_NotificationOtherIcon                 "MonokaiLightBlue"
	#define DarkThemeColor_NotificationWarningBack               "MonokaiDarkGray"
	#define DarkThemeColor_NotificationWarningBorder             "MonokaiLightGray"
	#define DarkThemeColor_NotificationWarningText               "White"
	#define DarkThemeColor_NotificationWarningIcon               "MonokaiOrange"
	#define DarkThemeColor_NotificationErrorBack                 "MonokaiDarkGray"
	#define DarkThemeColor_NotificationErrorBorder               "MonokaiLightGray"
	#define DarkThemeColor_NotificationErrorText                 "White"
	#define DarkThemeColor_NotificationErrorIcon                 "MonokaiMagenta"
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                      Light Theme Colors                      |
// +--------------------------------------------------------------+
	#define LightThemeColor_OptionListBack                        "White"
	#define LightThemeColor_ScrollGutter                          "White"
	#define LightThemeColor_ScrollBar                             "LightGray"
	#define LightThemeColor_ScrollBarHover                        "Gray"
	#define LightThemeColor_ScrollBarGrab                         "Gray"
	#define LightThemeColor_TooltipBack                           "NearWhite"
	#define LightThemeColor_TooltipBorder                         "Transparent"
	#define LightThemeColor_TooltipText                           "DarkGray"
	#define LightThemeColor_TopmostBorder                         "Blue"
	#define LightThemeColor_TopbarBack                            "NearWhite"
	#define LightThemeColor_TopbarBorder                          "BlueWhite"
	#define LightThemeColor_TopbarPathText                        "Gray"
	#define LightThemeColor_DropdownBack                          "NearWhite"
	#define LightThemeColor_DropdownBorder                        "BlueWhite"
	#define LightThemeColor_OptionBackOff                         "Transparent"
	#define LightThemeColor_OptionBackOn                          "Blue"
	#define LightThemeColor_OptionBackOnHovered                   "LightBlueToBlue2"
	#define LightThemeColor_OptionBackOffHovered                  "LightBlue"
	#define LightThemeColor_OptionBackTurningOff                  "LightBlueToBlue1"
	#define LightThemeColor_OptionBackTurningOn                   "LightBlueToBlue1"
	#define LightThemeColor_OptionBorderOff                       "BlueWhite"
	#define LightThemeColor_OptionBorderOn                        "Blue"
	#define LightThemeColor_OptionBorderOffHovered                "LightBlueToBlue1"
	#define LightThemeColor_OptionBorderOnHovered                 "Blue"
	#define LightThemeColor_OptionBorderTurningOff                "LightBlueToBlue1"
	#define LightThemeColor_OptionBorderTurningOn                 "Blue"
	#define LightThemeColor_OptionNameTextOff                     "LightGray"
	#define LightThemeColor_OptionNameTextOn                      "DarkGray"
	#define LightThemeColor_OptionNameTextOffHovered              "LightGray"
	#define LightThemeColor_OptionNameTextOnHovered               "DarkGray"
	#define LightThemeColor_OptionNameTextTurningOff              "LightGray"
	#define LightThemeColor_OptionNameTextTurningOn               "DarkGray"
	#define LightThemeColor_OptionValueTextOff                    "LightGray"
	#define LightThemeColor_OptionValueTextOn                     "DarkGray"
	#define LightThemeColor_OptionValueTextOffHovered             "LightGray"
	#define LightThemeColor_OptionValueTextOnHovered              "DarkGray"
	#define LightThemeColor_OptionValueTextTurningOff             "LightGray"
	#define LightThemeColor_OptionValueTextTurningOn              "DarkGray"
	#define LightThemeColor_TopbarBtnBack                         "Transparent"
	#define LightThemeColor_TopbarBtnBackHover                    "LightBlue"
	#define LightThemeColor_TopbarBtnBackOpen                     "LightBlue"
	#define LightThemeColor_TopbarBtnBorder                       "Transparent"
	#define LightThemeColor_TopbarBtnBorderHover                  "Blue"
	#define LightThemeColor_TopbarBtnBorderOpen                   "Transparent"
	#define LightThemeColor_TopbarBtnText                         "DarkGray"
	#define LightThemeColor_TopbarBtnTextHover                    "DarkGray"
	#define LightThemeColor_TopbarBtnTextOpen                     "DarkGray"
	#define LightThemeColor_DropdownBtnBack                       "Transparent"
	#define LightThemeColor_DropdownBtnBackHover                  "LightBlue"
	#define LightThemeColor_DropdownBtnBackPressed                "Blue"
	#define LightThemeColor_DropdownBtnBackOpen                   "LightBlue"
	#define LightThemeColor_DropdownBtnBackDisabled               "White"
	#define LightThemeColor_DropdownBtnBorder                     "Transparent"
	#define LightThemeColor_DropdownBtnBorderHover                "Blue"
	#define LightThemeColor_DropdownBtnBorderPressed              "Transparent"
	#define LightThemeColor_DropdownBtnBorderOpen                 "Transparent"
	#define LightThemeColor_DropdownBtnBorderDisabled             "Transparent"
	#define LightThemeColor_DropdownBtnText                       "DarkGray"
	#define LightThemeColor_DropdownBtnTextHover                  "DarkGray"
	#define LightThemeColor_DropdownBtnTextPressed                "DarkGray"
	#define LightThemeColor_DropdownBtnTextOpen                   "DarkGray"
	#define LightThemeColor_DropdownBtnTextDisabled               "LightGray"
	#define LightThemeColor_DropdownBtnIcon                       "DarkGray"
	#define LightThemeColor_DropdownBtnIconHover                  "DarkGray"
	#define LightThemeColor_DropdownBtnIconPressed                "DarkGray"
	#define LightThemeColor_DropdownBtnIconOpen                   "DarkGray"
	#define LightThemeColor_DropdownBtnIconDisabled               "LightGray"
	#define LightThemeColor_HotkeyBack                            "Transparent"
	#define LightThemeColor_HotkeyBackHover                       "Transparent"
	#define LightThemeColor_HotkeyBackPressed                     "Transparent"
	#define LightThemeColor_HotkeyBackDisabled                    "Transparent"
	#define LightThemeColor_HotkeyBorder                          "LightGray"
	#define LightThemeColor_HotkeyBorderHover                     "DarkGray"
	#define LightThemeColor_HotkeyBorderPressed                   "DarkGray"
	#define LightThemeColor_HotkeyBorderDisabled                  "LightGray"
	#define LightThemeColor_HotkeyText                            "LightGray"
	#define LightThemeColor_HotkeyTextHover                       "DarkGray"
	#define LightThemeColor_HotkeyTextPressed                     "DarkGray"
	#define LightThemeColor_HotkeyTextDisabled                    "LightGray"
	#define LightThemeColor_FileTabDivider                        "LightGray"
	#define LightThemeColor_FileTabBack                           "NearWhite"
	#define LightThemeColor_FileTabBackHover                      "BlueWhite"
	#define LightThemeColor_FileTabBackOpen                       "White"
	#define LightThemeColor_FileTabBorder                         "Transparent"
	#define LightThemeColor_FileTabBorderHover                    "Blue"
	#define LightThemeColor_FileTabBorderOpen                     "Transparent"
	#define LightThemeColor_FileTabText                           "Gray"
	#define LightThemeColor_FileTabTextHover                      "DarkGray"
	#define LightThemeColor_FileTabTextOpen                       "DarkGray"
	#define LightThemeColor_ConfirmDialogDarken                   "Darken25"
	#define LightThemeColor_ConfirmDialogBack                     "NearWhite"
	#define LightThemeColor_ConfirmDialogBorder                   "Gray"
	#define LightThemeColor_ConfirmDialogText                     "DarkGray"
	#define LightThemeColor_ConfirmDialogPositiveBtnBack          "Transparent"
	#define LightThemeColor_ConfirmDialogPositiveBtnBackHover     "Blue50"
	#define LightThemeColor_ConfirmDialogPositiveBtnBackPressed   "Blue50"
	#define LightThemeColor_ConfirmDialogPositiveBtnBorder        "Blue50"
	#define LightThemeColor_ConfirmDialogPositiveBtnBorderHover   "Blue"
	#define LightThemeColor_ConfirmDialogPositiveBtnBorderPressed "Blue"
	#define LightThemeColor_ConfirmDialogPositiveBtnText          "DarkGray"
	#define LightThemeColor_ConfirmDialogPositiveBtnTextHover     "DarkGray"
	#define LightThemeColor_ConfirmDialogPositiveBtnTextPressed   "DarkGray"
	#define LightThemeColor_ConfirmDialogNegativeBtnBack          "Transparent"
	#define LightThemeColor_ConfirmDialogNegativeBtnBackHover     "Red50"
	#define LightThemeColor_ConfirmDialogNegativeBtnBackPressed   "Red50"
	#define LightThemeColor_ConfirmDialogNegativeBtnBorder        "Red50"
	#define LightThemeColor_ConfirmDialogNegativeBtnBorderHover   "Red"
	#define LightThemeColor_ConfirmDialogNegativeBtnBorderPressed "Red"
	#define LightThemeColor_ConfirmDialogNegativeBtnText          "DarkGray"
	#define LightThemeColor_ConfirmDialogNegativeBtnTextHover     "DarkGray"
	#define LightThemeColor_ConfirmDialogNegativeBtnTextPressed   "DarkGray"
	#define LightThemeColor_ConfirmDialogNeutralBtnBack           "Transparent"
	#define LightThemeColor_ConfirmDialogNeutralBtnBackHover      "LightGray50"
	#define LightThemeColor_ConfirmDialogNeutralBtnBackPressed    "LightGray50"
	#define LightThemeColor_ConfirmDialogNeutralBtnBorder         "LightGray50"
	#define LightThemeColor_ConfirmDialogNeutralBtnBorderHover    "LightGray"
	#define LightThemeColor_ConfirmDialogNeutralBtnBorderPressed  "LightGray"
	#define LightThemeColor_ConfirmDialogNeutralBtnText           "DarkGray"
	#define LightThemeColor_ConfirmDialogNeutralBtnTextHover      "DarkGray"
	#define LightThemeColor_ConfirmDialogNeutralBtnTextPressed    "DarkGray"
	#define LightThemeColor_NotificationDebugBack                 "MonokaiDarkGray"
	#define LightThemeColor_NotificationDebugBorder               "MonokaiLightGray"
	#define LightThemeColor_NotificationDebugText                 "DarkGray"
	#define LightThemeColor_NotificationDebugIcon                 "MonokaiGray1"
	#define LightThemeColor_NotificationRegularBack               "MonokaiDarkGray"
	#define LightThemeColor_NotificationRegularBorder             "MonokaiLightGray"
	#define LightThemeColor_NotificationRegularText               "DarkGray"
	#define LightThemeColor_NotificationRegularIcon               "DarkGray"
	#define LightThemeColor_NotificationInfoBack                  "MonokaiDarkGray"
	#define LightThemeColor_NotificationInfoBorder                "MonokaiLightGray"
	#define LightThemeColor_NotificationInfoText                  "DarkGray"
	#define LightThemeColor_NotificationInfoIcon                  "MonokaiGreen"
	#define LightThemeColor_NotificationNotifyBack                "MonokaiDarkGray"
	#define LightThemeColor_NotificationNotifyBorder              "MonokaiLightGray"
	#define LightThemeColor_NotificationNotifyText                "DarkGray"
	#define LightThemeColor_NotificationNotifyIcon                "MonokaiPurple"
	#define LightThemeColor_NotificationOtherBack                 "MonokaiDarkGray"
	#define LightThemeColor_NotificationOtherBorder               "MonokaiLightGray"
	#define LightThemeColor_NotificationOtherText                 "DarkGray"
	#define LightThemeColor_NotificationOtherIcon                 "MonokaiLightBlue"
	#define LightThemeColor_NotificationWarningBack               "MonokaiDarkGray"
	#define LightThemeColor_NotificationWarningBorder             "MonokaiLightGray"
	#define LightThemeColor_NotificationWarningText               "DarkGray"
	#define LightThemeColor_NotificationWarningIcon               "MonokaiOrange"
	#define LightThemeColor_NotificationErrorBack                 "MonokaiDarkGray"
	#define LightThemeColor_NotificationErrorBorder               "MonokaiLightGray"
	#define LightThemeColor_NotificationErrorText                 "DarkGray"
	#define LightThemeColor_NotificationErrorIcon                 "MonokaiMagenta"
// +--------------------------------------------------------------+

// #define DarkThemeColor_BackgroundBlack    MakeColorU32(0xFF191B1C)
// #define DarkThemeColor_BackgroundDarkGray MakeColorU32(0xFF1F2223)
// #define DarkThemeColor_BackgroundGray     MakeColorU32(0xFF272A2B)
// #define DarkThemeColor_TextWhite          MakeColorU32(0xFFDDDEE0)
// #define DarkThemeColor_TextLightGray      MakeColorU32(0xFFAFB1B3)
// #define DarkThemeColor_TextGray           MakeColorU32(0xFF6B7078)
// #define DarkThemeColor_Outline            MakeColorU32(0xFF343A3B)
// #define DarkThemeColor_Hovered            MakeColorU32(0xFF103C4C)
// #define DarkThemeColor_Selected           MakeColorU32(0xFF0079A6)
// #define DarkThemeColor_Error              MakeColorU32(0xFFFF6666)

// #define LightThemeColor_BackgroundBlack    MakeColorU32(0xFFFFFFFF)
// #define LightThemeColor_BackgroundDarkGray MakeColorU32(0xFFF8F8F8)
// #define LightThemeColor_BackgroundGray     MakeColorU32(0xFFEBEBEB)
// #define LightThemeColor_TextWhite          MakeColorU32(0xFF3E3F40)
// #define LightThemeColor_TextLightGray      MakeColorU32(0xFF606263)
// #define LightThemeColor_TextGray           MakeColorU32(0xFF909293)
// #define LightThemeColor_Outline            MakeColorU32(0xFFDADADB)
// #define LightThemeColor_Hovered            MakeColorU32(0xFF99DEF8)
// #define LightThemeColor_Selected           MakeColorU32(0xFF00ADEE)
// #define LightThemeColor_Error              MakeColorU32(0xFFFF6666)

// #define DebugThemeColor_BackgroundBlack    PalRed    //FFF92672
// #define DebugThemeColor_BackgroundDarkGray PalPink   //FFED4E95
// #define DebugThemeColor_BackgroundGray     PalOrange //FFFD971F
// #define DebugThemeColor_TextWhite          PalHoney  //FFFDC459
// #define DebugThemeColor_TextLightGray      PalBanana //FFEEE64F
// #define DebugThemeColor_TextGray           PalGreen  //FFA6E22E
// #define DebugThemeColor_Outline            PalGrass  //FF92BB41
// #define DebugThemeColor_Hovered            PalBlue   //FF669BEF
// #define DebugThemeColor_Selected           PalSky    //FF66D9EF
// #define DebugThemeColor_Error              PalPurple //FFAE81FF

// X(EnumName)
#define Theme_XList(X)                       \
	X(OptionListBack)                        \
	X(ScrollGutter)                          \
	X(ScrollBar)                             \
	X(ScrollBarHover)                        \
	X(ScrollBarGrab)                         \
	X(TooltipBack)                           \
	X(TooltipBorder)                         \
	X(TooltipText)                           \
	X(TopmostBorder)                         \
	X(TopbarBack)                            \
	X(TopbarBorder)                          \
	X(TopbarPathText)                        \
	X(DropdownBack)                          \
	X(DropdownBorder)                        \
	X(OptionBackOff)                         \
	X(OptionBackOn)                          \
	X(OptionBackOnHovered)                   \
	X(OptionBackOffHovered)                  \
	X(OptionBackTurningOff)                  \
	X(OptionBackTurningOn)                   \
	X(OptionBorderOff)                       \
	X(OptionBorderOn)                        \
	X(OptionBorderOffHovered)                \
	X(OptionBorderOnHovered)                 \
	X(OptionBorderTurningOff)                \
	X(OptionBorderTurningOn)                 \
	X(OptionNameTextOff)                     \
	X(OptionNameTextOn)                      \
	X(OptionNameTextOffHovered)              \
	X(OptionNameTextOnHovered)               \
	X(OptionNameTextTurningOff)              \
	X(OptionNameTextTurningOn)               \
	X(OptionValueTextOff)                    \
	X(OptionValueTextOn)                     \
	X(OptionValueTextOffHovered)             \
	X(OptionValueTextOnHovered)              \
	X(OptionValueTextTurningOff)             \
	X(OptionValueTextTurningOn)              \
	X(TopbarBtnBack)                         \
	X(TopbarBtnBackHover)                    \
	X(TopbarBtnBackOpen)                     \
	X(TopbarBtnBorder)                       \
	X(TopbarBtnBorderHover)                  \
	X(TopbarBtnBorderOpen)                   \
	X(TopbarBtnText)                         \
	X(TopbarBtnTextHover)                    \
	X(TopbarBtnTextOpen)                     \
	X(DropdownBtnBack)                       \
	X(DropdownBtnBackHover)                  \
	X(DropdownBtnBackPressed)                \
	X(DropdownBtnBackOpen)                   \
	X(DropdownBtnBackDisabled)               \
	X(DropdownBtnBorder)                     \
	X(DropdownBtnBorderHover)                \
	X(DropdownBtnBorderPressed)              \
	X(DropdownBtnBorderOpen)                 \
	X(DropdownBtnBorderDisabled)             \
	X(DropdownBtnText)                       \
	X(DropdownBtnTextHover)                  \
	X(DropdownBtnTextPressed)                \
	X(DropdownBtnTextOpen)                   \
	X(DropdownBtnTextDisabled)               \
	X(DropdownBtnIcon)                       \
	X(DropdownBtnIconHover)                  \
	X(DropdownBtnIconPressed)                \
	X(DropdownBtnIconOpen)                   \
	X(DropdownBtnIconDisabled)               \
	X(HotkeyBack)                            \
	X(HotkeyBackHover)                       \
	X(HotkeyBackPressed)                     \
	X(HotkeyBackDisabled)                    \
	X(HotkeyBorder)                          \
	X(HotkeyBorderHover)                     \
	X(HotkeyBorderPressed)                   \
	X(HotkeyBorderDisabled)                  \
	X(HotkeyText)                            \
	X(HotkeyTextHover)                       \
	X(HotkeyTextPressed)                     \
	X(HotkeyTextDisabled)                    \
	X(FileTabDivider)                        \
	X(FileTabBack)                           \
	X(FileTabBackHover)                      \
	X(FileTabBackOpen)                       \
	X(FileTabBorder)                         \
	X(FileTabBorderHover)                    \
	X(FileTabBorderOpen)                     \
	X(FileTabText)                           \
	X(FileTabTextHover)                      \
	X(FileTabTextOpen)                       \
	X(ConfirmDialogDarken)                   \
	X(ConfirmDialogBack)                     \
	X(ConfirmDialogBorder)                   \
	X(ConfirmDialogText)                     \
	X(ConfirmDialogPositiveBtnBack)          \
	X(ConfirmDialogPositiveBtnBackHover)     \
	X(ConfirmDialogPositiveBtnBackPressed)   \
	X(ConfirmDialogPositiveBtnBorder)        \
	X(ConfirmDialogPositiveBtnBorderHover)   \
	X(ConfirmDialogPositiveBtnBorderPressed) \
	X(ConfirmDialogPositiveBtnText)          \
	X(ConfirmDialogPositiveBtnTextHover)     \
	X(ConfirmDialogPositiveBtnTextPressed)   \
	X(ConfirmDialogNegativeBtnBack)          \
	X(ConfirmDialogNegativeBtnBackHover)     \
	X(ConfirmDialogNegativeBtnBackPressed)   \
	X(ConfirmDialogNegativeBtnBorder)        \
	X(ConfirmDialogNegativeBtnBorderHover)   \
	X(ConfirmDialogNegativeBtnBorderPressed) \
	X(ConfirmDialogNegativeBtnText)          \
	X(ConfirmDialogNegativeBtnTextHover)     \
	X(ConfirmDialogNegativeBtnTextPressed)   \
	X(ConfirmDialogNeutralBtnBack)           \
	X(ConfirmDialogNeutralBtnBackHover)      \
	X(ConfirmDialogNeutralBtnBackPressed)    \
	X(ConfirmDialogNeutralBtnBorder)         \
	X(ConfirmDialogNeutralBtnBorderHover)    \
	X(ConfirmDialogNeutralBtnBorderPressed)  \
	X(ConfirmDialogNeutralBtnText)           \
	X(ConfirmDialogNeutralBtnTextHover)      \
	X(ConfirmDialogNeutralBtnTextPressed)    \
	X(NotificationDebugBack)                 \
	X(NotificationDebugBorder)               \
	X(NotificationDebugText)                 \
	X(NotificationDebugIcon)                 \
	X(NotificationRegularBack)               \
	X(NotificationRegularBorder)             \
	X(NotificationRegularText)               \
	X(NotificationRegularIcon)               \
	X(NotificationInfoBack)                  \
	X(NotificationInfoBorder)                \
	X(NotificationInfoText)                  \
	X(NotificationInfoIcon)                  \
	X(NotificationNotifyBack)                \
	X(NotificationNotifyBorder)              \
	X(NotificationNotifyText)                \
	X(NotificationNotifyIcon)                \
	X(NotificationOtherBack)                 \
	X(NotificationOtherBorder)               \
	X(NotificationOtherText)                 \
	X(NotificationOtherIcon)                 \
	X(NotificationWarningBack)               \
	X(NotificationWarningBorder)             \
	X(NotificationWarningText)               \
	X(NotificationWarningIcon)               \
	X(NotificationErrorBack)                 \
	X(NotificationErrorBorder)               \
	X(NotificationErrorText)                 \
	X(NotificationErrorIcon)

typedef enum ThemeColor ThemeColor;
enum ThemeColor
{
	ThemeColor_None = 0,
	
	#define X(EnumName) ThemeColor_##EnumName,
	Theme_XList(X)
	#undef X
	
	ThemeColor_Count,
};
const char* GetThemeColorStr(ThemeColor enumValue)
{
	switch (enumValue)
	{
		case ThemeColor_None: return "None";
		
		#define X(EnumName) case ThemeColor_##EnumName: return #EnumName;
		Theme_XList(X)
		#undef X
		
		default: return UNKNOWN_STR;
	}
}
bool TryParseThemeColor(Str8 themeColorStr, ThemeColor* colorOut)
{
	if (StrAnyCaseEquals(themeColorStr, StrLit("None"))) { SetOptionalOutPntr(colorOut, ThemeColor_None); return true; }
	
	#define X(EnumName) else if (StrAnyCaseEquals(themeColorStr, StrLit(#EnumName))) { SetOptionalOutPntr(colorOut, ThemeColor_##EnumName); return true; }
	Theme_XList(X)
	#undef X
	
	else { return false; }
}

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

typedef plex BakedTheme BakedTheme;
plex BakedTheme
{
	Color32 colors[ThemeColor_Count];
};

typedef plex UserThemeEntry UserThemeEntry;
plex UserThemeEntry
{
	ThemeMode mode;
	Str8 key;
	bool isStrValue;
	bool isResolved;
	bool isReferenced;
	Color32 valueColor;
	Str8 valueStr;
};

typedef plex UserTheme UserTheme;
plex UserTheme
{
	Arena* arena;
	VarArray entries; //UserThemeEntry
};

UserThemeEntry* FindUserThemeEntry(UserTheme* theme, ThemeMode mode, Str8 key)
{
	VarArrayLoop(&theme->entries, eIndex)
	{
		VarArrayLoopGet(UserThemeEntry, entry, &theme->entries, eIndex);
		if ((entry->mode == ThemeMode_None || entry->mode == mode) &&
			StrExactEquals(entry->key, key))
		{
			return entry;
		}
	}
	return nullptr;
}

// Returns false if an entry already existed and is getting overridden
inline bool AddUserThemeEntryColor(UserTheme* theme, ThemeMode mode, Str8 key, Color32 color)
{
	NotNull(theme);
	NotNull(theme->arena);
	UserThemeEntry* existingEntry = FindUserThemeEntry(theme, mode, key);
	if (existingEntry != nullptr)
	{
		if (existingEntry->isStrValue && CanArenaFree(theme->arena)) { FreeStr8(theme->arena, &existingEntry->valueStr); }
		existingEntry->isStrValue = false;
		existingEntry->valueColor = color;
		return false;
	}
	UserThemeEntry* newEntry = VarArrayAdd(UserThemeEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->isStrValue = false;
	newEntry->valueColor = color;
	return true;
}
inline bool AddUserThemeEntryStr(UserTheme* theme, ThemeMode mode, Str8 key, Str8 value)
{
	NotNull(theme);
	NotNull(theme->arena);
	UserThemeEntry* existingEntry = FindUserThemeEntry(theme, mode, key);
	if (existingEntry != nullptr)
	{
		if (existingEntry->isStrValue && CanArenaFree(theme->arena)) { FreeStr8(theme->arena, &existingEntry->valueStr); }
		existingEntry->isStrValue = true;
		existingEntry->valueStr = AllocStr8(theme->arena, value);
		return false;
	}
	UserThemeEntry* newEntry = VarArrayAdd(UserThemeEntry, &theme->entries);
	NotNull(newEntry);
	ClearPointer(newEntry);
	newEntry->mode = mode;
	newEntry->key = AllocStr8(theme->arena, key);
	newEntry->isStrValue = true;
	newEntry->valueStr = AllocStr8(theme->arena, value);
	return true;
}

inline void InitDefaultTheme(UserTheme* theme)
{
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("Transparent"),      ThemeVar_Transparent);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("Darken25"),         ThemeVar_Darken25);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiDarkGray"),  ThemeVar_MonokaiDarkGray);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiGray1"),     ThemeVar_MonokaiGray1);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiGreen"),     ThemeVar_MonokaiGreen);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiLightBlue"), ThemeVar_MonokaiLightBlue);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiLightGray"), ThemeVar_MonokaiLightGray);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiMagenta"),   ThemeVar_MonokaiMagenta);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiOrange"),    ThemeVar_MonokaiOrange);
	AddUserThemeEntryColor(theme, ThemeMode_None, StrLit("MonokaiPurple"),    ThemeVar_MonokaiPurple);
	
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Black"),           DarkThemeVar_Black);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("DarkGray"),        DarkThemeVar_DarkGray);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Gray"),            DarkThemeVar_Gray);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("White"),           DarkThemeVar_White);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("LighterGray"),     DarkThemeVar_LighterGray);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("LightGray"),       DarkThemeVar_LightGray);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("LightGray50"),     DarkThemeVar_LightGray50);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("DarkestBlue"),     DarkThemeVar_DarkestBlue);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("DarkBlue"),        DarkThemeVar_DarkBlue);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Blue"),            DarkThemeVar_Blue);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Blue50"),          DarkThemeVar_Blue50);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Red"),             DarkThemeVar_Red);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("Red50"),           DarkThemeVar_Red50);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("DarkBlueToBlue1"), DarkThemeVar_DarkBlueToBlue1);
	AddUserThemeEntryColor(theme, ThemeMode_Dark, StrLit("DarkBlueToBlue2"), DarkThemeVar_DarkBlueToBlue2);
	
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("White"),            LightThemeVar_White);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("NearWhite"),        LightThemeVar_NearWhite);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("DarkGray"),         LightThemeVar_DarkGray);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("Gray"),             LightThemeVar_Gray);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("LightGray"),        LightThemeVar_LightGray);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("LightGray50"),      LightThemeVar_LightGray50);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("BlueWhite"),        LightThemeVar_BlueWhite);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("LightBlue"),        LightThemeVar_LightBlue);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("Blue"),             LightThemeVar_Blue);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("Blue50"),           LightThemeVar_Blue50);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("Red"),              LightThemeVar_Red);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("Red50"),            LightThemeVar_Red50);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("LightBlueToBlue1"), LightThemeVar_LightBlueToBlue1);
	AddUserThemeEntryColor(theme, ThemeMode_Light, StrLit("LightBlueToBlue2"), LightThemeVar_LightBlueToBlue2);
	
	#define X(EnumName) AddUserThemeEntryStr(theme, ThemeMode_Dark, StrLit(#EnumName), StrLit(DarkThemeColor_##EnumName));
	Theme_XList(X)
	#undef X
	
	#define X(EnumName) AddUserThemeEntryStr(theme, ThemeMode_Light, StrLit(#EnumName), StrLit(LightThemeColor_##EnumName));
	Theme_XList(X)
	#undef X
	
	for (uxx cIndex = 1; cIndex < ThemeColor_Count; cIndex++)
	{
		AddUserThemeEntryColor(theme, ThemeMode_Debug, MakeStr8Nt(GetThemeColorStr((ThemeColor)cIndex)), GetPredefPalColorByIndex(cIndex));
	}
}

#endif //  _APP_THEME_H
