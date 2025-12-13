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

#define DarkThemeVar_Black           MakeColorU32(0xFF191B1C) //BackgroundBlack
#define DarkThemeVar_DarkGray        MakeColorU32(0xFF1F2223) //BackgroundDarkGray
#define DarkThemeVar_Gray            MakeColorU32(0xFF272A2B) //BackgroundGray
#define DarkThemeVar_White           MakeColorU32(0xFFDDDEE0) //TextWhite
#define DarkThemeVar_LighterGray     MakeColorU32(0xFFAFB1B3) //TextLightGray
#define DarkThemeVar_LightGray       MakeColorU32(0xFF6B7078) //TextGray
#define DarkThemeVar_LightGray50     MakeColorU32(0x806B7078)
#define DarkThemeVar_DarkestBlue     MakeColorU32(0xFF343A3B) //Outline
#define DarkThemeVar_DarkBlue        MakeColorU32(0xFF103C4C) //Hovered
#define DarkThemeVar_Blue            MakeColorU32(0xFF0079A6) //Selected
#define DarkThemeVar_Blue50          MakeColorU32(0x800079A6)
#define DarkThemeVar_Red             MakeColorU32(0xFFFF6666) //Error
#define DarkThemeVar_Red50           MakeColorU32(0x80FF6666)
#define DarkThemeVar_DarkBlueToBlue1 MakeColorU32(0xFF085B79)
#define DarkThemeVar_DarkBlueToBlue2 MakeColorU32(0xFF046A90)

#define LightThemeVar_Black           MakeColorU32(0xFFFFFFFF)
#define LightThemeVar_DarkGray        MakeColorU32(0xFFF8F8F8)
#define LightThemeVar_Gray            MakeColorU32(0xFFEBEBEB)
#define LightThemeVar_White           MakeColorU32(0xFF3E3F40)
#define LightThemeVar_LighterGray     MakeColorU32(0xFF606263)
#define LightThemeVar_LightGray       MakeColorU32(0xFF909293)
#define LightThemeVar_LightGray50     MakeColorU32(0x80909293)
#define LightThemeVar_DarkestBlue     MakeColorU32(0xFFDADADB)
#define LightThemeVar_DarkBlue        MakeColorU32(0xFF99DEF8)
#define LightThemeVar_Blue            MakeColorU32(0xFF00ADEE)
#define LightThemeVar_Blue50          MakeColorU32(0x8000ADEE)
#define LightThemeVar_Red             MakeColorU32(0xFFFF6666)
#define LightThemeVar_Red50           MakeColorU32(0x80FF6666)
#define LightThemeVar_DarkBlueToBlue1 MakeColorU32(0xFF4DC6F3)
#define LightThemeVar_DarkBlueToBlue2 MakeColorU32(0xFF26B9F1)

// +--------------------------------------------------------------+
// |                      Dark Theme Colors                       |
// +--------------------------------------------------------------+
	#define DarkThemeColor_OptionListBack                        DarkThemeVar_Black
	#define DarkThemeColor_ScrollGutter                          DarkThemeVar_Black
	#define DarkThemeColor_ScrollBar                             DarkThemeVar_DarkestBlue
	#define DarkThemeColor_ScrollBarHover                        DarkThemeVar_LightGray
	#define DarkThemeColor_ScrollBarGrab                         DarkThemeVar_LightGray
	#define DarkThemeColor_TooltipBack                           DarkThemeVar_White
	#define DarkThemeColor_TooltipBorder                         ThemeVar_Transparent
	#define DarkThemeColor_TooltipText                           DarkThemeVar_DarkGray
	#define DarkThemeColor_TopmostBorder                         DarkThemeVar_Blue
	#define DarkThemeColor_TopbarBack                            DarkThemeVar_Gray
	#define DarkThemeColor_TopbarBorder                          DarkThemeVar_DarkestBlue
	#define DarkThemeColor_TopbarPathText                        DarkThemeVar_LighterGray
	#define DarkThemeColor_DropdownBack                          DarkThemeVar_Gray
	#define DarkThemeColor_DropdownBorder                        DarkThemeVar_DarkestBlue
	#define DarkThemeColor_OptionBackOff                         ThemeVar_Transparent
	#define DarkThemeColor_OptionBackOn                          DarkThemeVar_Blue
	#define DarkThemeColor_OptionBackOnHovered                   DarkThemeVar_DarkBlueToBlue2
	#define DarkThemeColor_OptionBackOffHovered                  DarkThemeVar_DarkBlue
	#define DarkThemeColor_OptionBackTurningOff                  DarkThemeVar_DarkBlueToBlue1
	#define DarkThemeColor_OptionBackTurningOn                   DarkThemeVar_DarkBlueToBlue1
	#define DarkThemeColor_OptionBorderOff                       DarkThemeVar_DarkestBlue
	#define DarkThemeColor_OptionBorderOn                        DarkThemeVar_Blue
	#define DarkThemeColor_OptionBorderOffHovered                DarkThemeVar_DarkBlueToBlue1
	#define DarkThemeColor_OptionBorderOnHovered                 DarkThemeVar_Blue
	#define DarkThemeColor_OptionBorderTurningOff                DarkThemeVar_DarkBlueToBlue1
	#define DarkThemeColor_OptionBorderTurningOn                 DarkThemeVar_Blue
	#define DarkThemeColor_OptionNameTextOff                     DarkThemeVar_LightGray
	#define DarkThemeColor_OptionNameTextOn                      DarkThemeVar_White
	#define DarkThemeColor_OptionNameTextOffHovered              DarkThemeVar_LightGray
	#define DarkThemeColor_OptionNameTextOnHovered               DarkThemeVar_White
	#define DarkThemeColor_OptionNameTextTurningOff              DarkThemeVar_LightGray
	#define DarkThemeColor_OptionNameTextTurningOn               DarkThemeVar_White
	#define DarkThemeColor_OptionValueTextOff                    DarkThemeVar_LightGray
	#define DarkThemeColor_OptionValueTextOn                     DarkThemeVar_White
	#define DarkThemeColor_OptionValueTextOffHovered             DarkThemeVar_LightGray
	#define DarkThemeColor_OptionValueTextOnHovered              DarkThemeVar_White
	#define DarkThemeColor_OptionValueTextTurningOff             DarkThemeVar_LightGray
	#define DarkThemeColor_OptionValueTextTurningOn              DarkThemeVar_White
	#define DarkThemeColor_TopbarBtnBack                         ThemeVar_Transparent
	#define DarkThemeColor_TopbarBtnBackHover                    DarkThemeVar_DarkBlue
	#define DarkThemeColor_TopbarBtnBackOpen                     DarkThemeVar_DarkBlue
	#define DarkThemeColor_TopbarBtnBorder                       ThemeVar_Transparent
	#define DarkThemeColor_TopbarBtnBorderHover                  DarkThemeVar_Blue
	#define DarkThemeColor_TopbarBtnBorderOpen                   ThemeVar_Transparent
	#define DarkThemeColor_TopbarBtnText                         DarkThemeVar_White
	#define DarkThemeColor_TopbarBtnTextHover                    DarkThemeVar_White
	#define DarkThemeColor_TopbarBtnTextOpen                     DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnBack                       ThemeVar_Transparent
	#define DarkThemeColor_DropdownBtnBackHover                  DarkThemeVar_DarkBlue
	#define DarkThemeColor_DropdownBtnBackPressed                DarkThemeVar_Blue
	#define DarkThemeColor_DropdownBtnBackOpen                   DarkThemeVar_DarkBlue
	#define DarkThemeColor_DropdownBtnBackDisabled               DarkThemeVar_Black
	#define DarkThemeColor_DropdownBtnBorder                     ThemeVar_Transparent
	#define DarkThemeColor_DropdownBtnBorderHover                DarkThemeVar_Blue
	#define DarkThemeColor_DropdownBtnBorderPressed              ThemeVar_Transparent
	#define DarkThemeColor_DropdownBtnBorderOpen                 ThemeVar_Transparent
	#define DarkThemeColor_DropdownBtnBorderDisabled             ThemeVar_Transparent
	#define DarkThemeColor_DropdownBtnText                       DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnTextHover                  DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnTextPressed                DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnTextOpen                   DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnTextDisabled               DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnIcon                       DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnIconHover                  DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnIconPressed                DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnIconOpen                   DarkThemeVar_White
	#define DarkThemeColor_DropdownBtnIconDisabled               DarkThemeVar_White
	#define DarkThemeColor_HotkeyBack                            ThemeVar_Transparent
	#define DarkThemeColor_HotkeyBackHover                       ThemeVar_Transparent
	#define DarkThemeColor_HotkeyBackPressed                     ThemeVar_Transparent
	#define DarkThemeColor_HotkeyBackDisabled                    ThemeVar_Transparent
	#define DarkThemeColor_HotkeyBorder                          DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyBorderHover                     DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyBorderPressed                   DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyBorderDisabled                  DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyText                            DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyTextHover                       DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyTextPressed                     DarkThemeVar_LightGray
	#define DarkThemeColor_HotkeyTextDisabled                    DarkThemeVar_LightGray
	#define DarkThemeColor_FileTabDivider                        DarkThemeVar_LightGray
	#define DarkThemeColor_FileTabBack                           DarkThemeVar_Gray
	#define DarkThemeColor_FileTabBackHover                      DarkThemeVar_DarkestBlue
	#define DarkThemeColor_FileTabBackOpen                       DarkThemeVar_Black
	#define DarkThemeColor_FileTabBorder                         ThemeVar_Transparent
	#define DarkThemeColor_FileTabBorderHover                    DarkThemeVar_Blue
	#define DarkThemeColor_FileTabBorderOpen                     ThemeVar_Transparent
	#define DarkThemeColor_FileTabText                           DarkThemeVar_LighterGray
	#define DarkThemeColor_FileTabTextHover                      DarkThemeVar_White
	#define DarkThemeColor_FileTabTextOpen                       DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogDarken                   ThemeVar_Darken25
	#define DarkThemeColor_ConfirmDialogBack                     DarkThemeVar_Gray
	#define DarkThemeColor_ConfirmDialogBorder                   DarkThemeVar_LighterGray
	#define DarkThemeColor_ConfirmDialogText                     DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogPositiveBtnBack          ThemeVar_Transparent
	#define DarkThemeColor_ConfirmDialogPositiveBtnBackHover     DarkThemeVar_Blue50
	#define DarkThemeColor_ConfirmDialogPositiveBtnBackPressed   DarkThemeVar_Blue50
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorder        DarkThemeVar_Blue50
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorderHover   DarkThemeVar_Blue
	#define DarkThemeColor_ConfirmDialogPositiveBtnBorderPressed DarkThemeVar_Blue
	#define DarkThemeColor_ConfirmDialogPositiveBtnText          DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogPositiveBtnTextHover     DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogPositiveBtnTextPressed   DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNegativeBtnBack          ThemeVar_Transparent
	#define DarkThemeColor_ConfirmDialogNegativeBtnBackHover     DarkThemeVar_Red50
	#define DarkThemeColor_ConfirmDialogNegativeBtnBackPressed   DarkThemeVar_Red50
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorder        DarkThemeVar_Red50
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorderHover   DarkThemeVar_Red
	#define DarkThemeColor_ConfirmDialogNegativeBtnBorderPressed DarkThemeVar_Red
	#define DarkThemeColor_ConfirmDialogNegativeBtnText          DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNegativeBtnTextHover     DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNegativeBtnTextPressed   DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNeutralBtnBack           ThemeVar_Transparent
	#define DarkThemeColor_ConfirmDialogNeutralBtnBackHover      DarkThemeVar_LightGray50
	#define DarkThemeColor_ConfirmDialogNeutralBtnBackPressed    DarkThemeVar_LightGray50
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorder         DarkThemeVar_LightGray50
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorderHover    DarkThemeVar_LightGray
	#define DarkThemeColor_ConfirmDialogNeutralBtnBorderPressed  DarkThemeVar_LightGray
	#define DarkThemeColor_ConfirmDialogNeutralBtnText           DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNeutralBtnTextHover      DarkThemeVar_White
	#define DarkThemeColor_ConfirmDialogNeutralBtnTextPressed    DarkThemeVar_White
	#define DarkThemeColor_NotificationDebugBack                 ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationDebugBorder               ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationDebugText                 DarkThemeVar_White
	#define DarkThemeColor_NotificationDebugIcon                 ThemeVar_MonokaiGray1
	#define DarkThemeColor_NotificationRegularBack               ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationRegularBorder             ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationRegularText               DarkThemeVar_White
	#define DarkThemeColor_NotificationRegularIcon               DarkThemeVar_White
	#define DarkThemeColor_NotificationInfoBack                  ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationInfoBorder                ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationInfoText                  DarkThemeVar_White
	#define DarkThemeColor_NotificationInfoIcon                  ThemeVar_MonokaiGreen
	#define DarkThemeColor_NotificationNotifyBack                ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationNotifyBorder              ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationNotifyText                DarkThemeVar_White
	#define DarkThemeColor_NotificationNotifyIcon                ThemeVar_MonokaiPurple
	#define DarkThemeColor_NotificationOtherBack                 ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationOtherBorder               ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationOtherText                 DarkThemeVar_White
	#define DarkThemeColor_NotificationOtherIcon                 ThemeVar_MonokaiLightBlue
	#define DarkThemeColor_NotificationWarningBack               ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationWarningBorder             ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationWarningText               DarkThemeVar_White
	#define DarkThemeColor_NotificationWarningIcon               ThemeVar_MonokaiOrange
	#define DarkThemeColor_NotificationErrorBack                 ThemeVar_MonokaiDarkGray
	#define DarkThemeColor_NotificationErrorBorder               ThemeVar_MonokaiLightGray
	#define DarkThemeColor_NotificationErrorText                 DarkThemeVar_White
	#define DarkThemeColor_NotificationErrorIcon                 ThemeVar_MonokaiMagenta
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                      Light Theme Colors                      |
// +--------------------------------------------------------------+
	#define LightThemeColor_OptionListBack                        LightThemeVar_Black
	#define LightThemeColor_ScrollGutter                          LightThemeVar_Black
	#define LightThemeColor_ScrollBar                             LightThemeVar_DarkestBlue
	#define LightThemeColor_ScrollBarHover                        LightThemeVar_LightGray
	#define LightThemeColor_ScrollBarGrab                         LightThemeVar_LightGray
	#define LightThemeColor_TooltipBack                           LightThemeVar_White
	#define LightThemeColor_TooltipBorder                         ThemeVar_Transparent
	#define LightThemeColor_TooltipText                           LightThemeVar_DarkGray
	#define LightThemeColor_TopmostBorder                         LightThemeVar_Blue
	#define LightThemeColor_TopbarBack                            LightThemeVar_Gray
	#define LightThemeColor_TopbarBorder                          LightThemeVar_DarkestBlue
	#define LightThemeColor_TopbarPathText                        LightThemeVar_LighterGray
	#define LightThemeColor_DropdownBack                          LightThemeVar_Gray
	#define LightThemeColor_DropdownBorder                        LightThemeVar_DarkestBlue
	#define LightThemeColor_OptionBackOff                         ThemeVar_Transparent
	#define LightThemeColor_OptionBackOn                          LightThemeVar_Blue
	#define LightThemeColor_OptionBackOnHovered                   LightThemeVar_DarkBlueToBlue2
	#define LightThemeColor_OptionBackOffHovered                  LightThemeVar_DarkBlue
	#define LightThemeColor_OptionBackTurningOff                  LightThemeVar_DarkBlueToBlue1
	#define LightThemeColor_OptionBackTurningOn                   LightThemeVar_DarkBlueToBlue1
	#define LightThemeColor_OptionBorderOff                       LightThemeVar_DarkestBlue
	#define LightThemeColor_OptionBorderOn                        LightThemeVar_Blue
	#define LightThemeColor_OptionBorderOffHovered                LightThemeVar_DarkBlueToBlue1
	#define LightThemeColor_OptionBorderOnHovered                 LightThemeVar_Blue
	#define LightThemeColor_OptionBorderTurningOff                LightThemeVar_DarkBlueToBlue1
	#define LightThemeColor_OptionBorderTurningOn                 LightThemeVar_Blue
	#define LightThemeColor_OptionNameTextOff                     LightThemeVar_LightGray
	#define LightThemeColor_OptionNameTextOn                      LightThemeVar_White
	#define LightThemeColor_OptionNameTextOffHovered              LightThemeVar_LightGray
	#define LightThemeColor_OptionNameTextOnHovered               LightThemeVar_White
	#define LightThemeColor_OptionNameTextTurningOff              LightThemeVar_LightGray
	#define LightThemeColor_OptionNameTextTurningOn               LightThemeVar_White
	#define LightThemeColor_OptionValueTextOff                    LightThemeVar_LightGray
	#define LightThemeColor_OptionValueTextOn                     LightThemeVar_White
	#define LightThemeColor_OptionValueTextOffHovered             LightThemeVar_LightGray
	#define LightThemeColor_OptionValueTextOnHovered              LightThemeVar_White
	#define LightThemeColor_OptionValueTextTurningOff             LightThemeVar_LightGray
	#define LightThemeColor_OptionValueTextTurningOn              LightThemeVar_White
	#define LightThemeColor_TopbarBtnBack                         ThemeVar_Transparent
	#define LightThemeColor_TopbarBtnBackHover                    LightThemeVar_DarkBlue
	#define LightThemeColor_TopbarBtnBackOpen                     LightThemeVar_DarkBlue
	#define LightThemeColor_TopbarBtnBorder                       ThemeVar_Transparent
	#define LightThemeColor_TopbarBtnBorderHover                  LightThemeVar_Blue
	#define LightThemeColor_TopbarBtnBorderOpen                   ThemeVar_Transparent
	#define LightThemeColor_TopbarBtnText                         LightThemeVar_White
	#define LightThemeColor_TopbarBtnTextHover                    LightThemeVar_White
	#define LightThemeColor_TopbarBtnTextOpen                     LightThemeVar_White
	#define LightThemeColor_DropdownBtnBack                       ThemeVar_Transparent
	#define LightThemeColor_DropdownBtnBackHover                  LightThemeVar_DarkBlue
	#define LightThemeColor_DropdownBtnBackPressed                LightThemeVar_Blue
	#define LightThemeColor_DropdownBtnBackOpen                   LightThemeVar_DarkBlue
	#define LightThemeColor_DropdownBtnBackDisabled               LightThemeVar_Black
	#define LightThemeColor_DropdownBtnBorder                     ThemeVar_Transparent
	#define LightThemeColor_DropdownBtnBorderHover                LightThemeVar_Blue
	#define LightThemeColor_DropdownBtnBorderPressed              ThemeVar_Transparent
	#define LightThemeColor_DropdownBtnBorderOpen                 ThemeVar_Transparent
	#define LightThemeColor_DropdownBtnBorderDisabled             ThemeVar_Transparent
	#define LightThemeColor_DropdownBtnText                       LightThemeVar_White
	#define LightThemeColor_DropdownBtnTextHover                  LightThemeVar_White
	#define LightThemeColor_DropdownBtnTextPressed                LightThemeVar_White
	#define LightThemeColor_DropdownBtnTextOpen                   LightThemeVar_White
	#define LightThemeColor_DropdownBtnTextDisabled               LightThemeVar_White
	#define LightThemeColor_DropdownBtnIcon                       LightThemeVar_White
	#define LightThemeColor_DropdownBtnIconHover                  LightThemeVar_White
	#define LightThemeColor_DropdownBtnIconPressed                LightThemeVar_White
	#define LightThemeColor_DropdownBtnIconOpen                   LightThemeVar_White
	#define LightThemeColor_DropdownBtnIconDisabled               LightThemeVar_White
	#define LightThemeColor_HotkeyBack                            ThemeVar_Transparent
	#define LightThemeColor_HotkeyBackHover                       ThemeVar_Transparent
	#define LightThemeColor_HotkeyBackPressed                     ThemeVar_Transparent
	#define LightThemeColor_HotkeyBackDisabled                    ThemeVar_Transparent
	#define LightThemeColor_HotkeyBorder                          LightThemeVar_LightGray
	#define LightThemeColor_HotkeyBorderHover                     LightThemeVar_LightGray
	#define LightThemeColor_HotkeyBorderPressed                   LightThemeVar_LightGray
	#define LightThemeColor_HotkeyBorderDisabled                  LightThemeVar_LightGray
	#define LightThemeColor_HotkeyText                            LightThemeVar_LightGray
	#define LightThemeColor_HotkeyTextHover                       LightThemeVar_LightGray
	#define LightThemeColor_HotkeyTextPressed                     LightThemeVar_LightGray
	#define LightThemeColor_HotkeyTextDisabled                    LightThemeVar_LightGray
	#define LightThemeColor_FileTabDivider                        LightThemeVar_LightGray
	#define LightThemeColor_FileTabBack                           LightThemeVar_Gray
	#define LightThemeColor_FileTabBackHover                      LightThemeVar_DarkestBlue
	#define LightThemeColor_FileTabBackOpen                       LightThemeVar_Black
	#define LightThemeColor_FileTabBorder                         ThemeVar_Transparent
	#define LightThemeColor_FileTabBorderHover                    LightThemeVar_Blue
	#define LightThemeColor_FileTabBorderOpen                     ThemeVar_Transparent
	#define LightThemeColor_FileTabText                           LightThemeVar_LighterGray
	#define LightThemeColor_FileTabTextHover                      LightThemeVar_White
	#define LightThemeColor_FileTabTextOpen                       LightThemeVar_White
	#define LightThemeColor_ConfirmDialogDarken                   ThemeVar_Darken25
	#define LightThemeColor_ConfirmDialogBack                     LightThemeVar_Gray
	#define LightThemeColor_ConfirmDialogBorder                   LightThemeVar_LighterGray
	#define LightThemeColor_ConfirmDialogText                     LightThemeVar_White
	#define LightThemeColor_ConfirmDialogPositiveBtnBack          ThemeVar_Transparent
	#define LightThemeColor_ConfirmDialogPositiveBtnBackHover     LightThemeVar_Blue50
	#define LightThemeColor_ConfirmDialogPositiveBtnBackPressed   LightThemeVar_Blue50
	#define LightThemeColor_ConfirmDialogPositiveBtnBorder        LightThemeVar_Blue50
	#define LightThemeColor_ConfirmDialogPositiveBtnBorderHover   LightThemeVar_Blue
	#define LightThemeColor_ConfirmDialogPositiveBtnBorderPressed LightThemeVar_Blue
	#define LightThemeColor_ConfirmDialogPositiveBtnText          LightThemeVar_White
	#define LightThemeColor_ConfirmDialogPositiveBtnTextHover     LightThemeVar_White
	#define LightThemeColor_ConfirmDialogPositiveBtnTextPressed   LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNegativeBtnBack          ThemeVar_Transparent
	#define LightThemeColor_ConfirmDialogNegativeBtnBackHover     LightThemeVar_Red50
	#define LightThemeColor_ConfirmDialogNegativeBtnBackPressed   LightThemeVar_Red50
	#define LightThemeColor_ConfirmDialogNegativeBtnBorder        LightThemeVar_Red50
	#define LightThemeColor_ConfirmDialogNegativeBtnBorderHover   LightThemeVar_Red
	#define LightThemeColor_ConfirmDialogNegativeBtnBorderPressed LightThemeVar_Red
	#define LightThemeColor_ConfirmDialogNegativeBtnText          LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNegativeBtnTextHover     LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNegativeBtnTextPressed   LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNeutralBtnBack           ThemeVar_Transparent
	#define LightThemeColor_ConfirmDialogNeutralBtnBackHover      LightThemeVar_LightGray50
	#define LightThemeColor_ConfirmDialogNeutralBtnBackPressed    LightThemeVar_LightGray50
	#define LightThemeColor_ConfirmDialogNeutralBtnBorder         LightThemeVar_LightGray50
	#define LightThemeColor_ConfirmDialogNeutralBtnBorderHover    LightThemeVar_LightGray
	#define LightThemeColor_ConfirmDialogNeutralBtnBorderPressed  LightThemeVar_LightGray
	#define LightThemeColor_ConfirmDialogNeutralBtnText           LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNeutralBtnTextHover      LightThemeVar_White
	#define LightThemeColor_ConfirmDialogNeutralBtnTextPressed    LightThemeVar_White
	#define LightThemeColor_NotificationDebugBack                 ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationDebugBorder               ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationDebugText                 LightThemeVar_White
	#define LightThemeColor_NotificationDebugIcon                 ThemeVar_MonokaiGray1
	#define LightThemeColor_NotificationRegularBack               ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationRegularBorder             ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationRegularText               LightThemeVar_White
	#define LightThemeColor_NotificationRegularIcon               LightThemeVar_White
	#define LightThemeColor_NotificationInfoBack                  ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationInfoBorder                ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationInfoText                  LightThemeVar_White
	#define LightThemeColor_NotificationInfoIcon                  ThemeVar_MonokaiGreen
	#define LightThemeColor_NotificationNotifyBack                ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationNotifyBorder              ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationNotifyText                LightThemeVar_White
	#define LightThemeColor_NotificationNotifyIcon                ThemeVar_MonokaiPurple
	#define LightThemeColor_NotificationOtherBack                 ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationOtherBorder               ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationOtherText                 LightThemeVar_White
	#define LightThemeColor_NotificationOtherIcon                 ThemeVar_MonokaiLightBlue
	#define LightThemeColor_NotificationWarningBack               ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationWarningBorder             ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationWarningText               LightThemeVar_White
	#define LightThemeColor_NotificationWarningIcon               ThemeVar_MonokaiOrange
	#define LightThemeColor_NotificationErrorBack                 ThemeVar_MonokaiDarkGray
	#define LightThemeColor_NotificationErrorBorder               ThemeVar_MonokaiLightGray
	#define LightThemeColor_NotificationErrorText                 LightThemeVar_White
	#define LightThemeColor_NotificationErrorIcon                 ThemeVar_MonokaiMagenta
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
	if      (StrAnyCaseEquals(themeColorStr, StrLit("None"))) { SetOptionalOutPntr(colorOut, ThemeColor_None);               return true; }
	
	#define X(EnumName) else if (StrAnyCaseEquals(themeColorStr, StrLit(#EnumName))) { SetOptionalOutPntr(colorOut, ThemeColor_##EnumName); return true; }
	Theme_XList(X)
	#undef X
	
	else                                                                    { return false; }
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
bool TryParsePresetTheme(Str8 presetThemeStr, PresetTheme* presetThemeOut)
{
	if      (StrAnyCaseEquals(presetThemeStr, StrLit("None")))  { SetOptionalOutPntr(presetThemeOut, PresetTheme_None);  return true; }
	else if (StrAnyCaseEquals(presetThemeStr, StrLit("Dark")))  { SetOptionalOutPntr(presetThemeOut, PresetTheme_Dark);  return true; }
	else if (StrAnyCaseEquals(presetThemeStr, StrLit("Light"))) { SetOptionalOutPntr(presetThemeOut, PresetTheme_Light); return true; }
	else if (StrAnyCaseEquals(presetThemeStr, StrLit("Debug"))) { SetOptionalOutPntr(presetThemeOut, PresetTheme_Debug); return true; }
	else                                                      { return false; }
}

inline void InitDarkThemePreset(Theme* theme)
{
	#define X(EnumName) SetThemeColor(theme, ThemeColor_##EnumName, DarkThemeColor_##EnumName);
	Theme_XList(X)
	#undef X
}
inline void InitLightThemePreset(Theme* theme)
{
	#define X(EnumName) SetThemeColor(theme, ThemeColor_##EnumName, LightThemeColor_##EnumName);
	Theme_XList(X)
	#undef X
}
inline void InitDebugThemePreset(Theme* theme)
{
	for (uxx cIndex = 0; cIndex < ThemeColor_Count; cIndex++)
	{
		SetThemeColor(theme, (ThemeColor)cIndex, GetPredefPalColorByIndex(cIndex));
	}
}

#endif //  _APP_THEME_H
