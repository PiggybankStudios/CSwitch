/*
File:   app_ui_widgets.c
Author: Taylor Robbins
Date:   06\10\2026
Description: 
	** Holds functions that encapsulate various "widgets" that we re-use in the UI.
	** Widgets are a combination of behavior and display, producing UI elements and
	** checking+handling user input for interactable widgets
*/

#if BUILD_WITH_PIG_UI

bool UiTopbar_(UiId topbarId, bool isEnabled)
{
	UiSizingAxis topbarHeight = isEnabled ? (UiSizingAxis)UI_FIT() : (UiSizingAxis)UI_FIXED(0.1f);
	bool isTopbarOpen = OpenUiElementConditional(NEW_STRUCT(UiElemConfig){ .id = topbarId,
		.direction = UiLayoutDir_LeftToRight,
		.alignment = UI_ALIGN_LEFT_CENTER(),
		.sizing = {
			.width = UI_EXPAND(),
			.height = topbarHeight,
		},
		//TODO: Do we need padding to account for border on bottom?
		.padding = { .inner={ .Bottom=1 }, .child = 2 },
		.color = GetThemeColor(TopbarBack),
		.borderColor = GetThemeColor(TopbarBorder),
		.borderThickness = { .Bottom=1 },
	});
	return isTopbarOpen;
}
#define UiTopbar(...) DeferIfBlockCondEnd(UiTopbar_(__VA_ARGS__), CloseUiElement())

bool UiTopbarMenuBtn_(UiId btnId, Str8 displayText, bool* isMenuOpen, bool* keepOpenUntilMouseover, bool isSubmenuOpen)
{
	//TODO: Add MakeSuffixId(baseId, suffixStr)
	UiId menuId = CalcUiId(btnId, StrLit("_Menu"), 0);
	PigFont* font = &app->uiFont;
	r32 fontSize = app->uiFontSize;
	u8 fontStyle = UI_FONT_STYLE;
	TextMeasure displayTextSize = MeasureTextEx(font, fontSize, fontStyle, false, 0, displayText);
	
	bool isBtnHovered = IsUiElementHovered(btnId);
	bool isMenuHovered = IsUiElementHovered(menuId);
	bool isBtnOrMenuHovered = (isBtnHovered || isMenuHovered);
	ThemeState btnThemeState = *isMenuOpen ? ThemeState_Open : (isBtnHovered ? ThemeState_Hovered : ThemeState_Default);
	Color32 backgroundColor = GetThemeColorEx(TopbarBtnBack, btnThemeState);
	Color32 borderColor = GetThemeColorEx(TopbarBtnBorder, btnThemeState);
	Color32 textColor = GetThemeColorEx(TopbarBtnText, btnThemeState);
	
	//TODO: The size of text-holding elements should be auto-calculated
	UIELEM({ .id = btnId,
		.sizing = UI_FIXED2((displayTextSize.Width / app->settings.uiScale) + 5*2, (displayTextSize.Height / app->settings.uiScale) + 2*2),
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1),
		//Add a radius of 4px
		// .padding = { .inner=MakeV4r(4, 2, 4, 2) },
	})
	{
		UIELEM_LEAF({
			.text = displayText,
			.font = font,
			.fontSize = fontSize,
			.fontStyle = fontStyle,
			.textColor = textColor,
			.sizing = UI_FIXED2((displayTextSize.Width / app->settings.uiScale), (displayTextSize.Height / app->settings.uiScale)),
		});
	}
	
	if (appIn->mouse.isOverWindow && isBtnHovered && MouseLeftClicked())
	{
		*isMenuOpen = !*isMenuOpen;
	}
	if (*isMenuOpen == true && isBtnOrMenuHovered && *keepOpenUntilMouseover) { *keepOpenUntilMouseover = false; } //once we are closed or the mouse is over, clear this flag, mouse leaving now will constitute closing
	if (*isMenuOpen == true && !isBtnOrMenuHovered && !*keepOpenUntilMouseover && !isSubmenuOpen) { *isMenuOpen = false; }
	
	bool menuNeedsClosing = false;
	if (*isMenuOpen)
	{
		OpenUiElement(NEW_STRUCT(UiElemConfig){ .id = menuId,
			.direction = UiLayoutDir_TopDown,
			.sizing = UI_FIT2(),
			.padding = { .inner=FillV4r(2), .child=2 },
			.color = GetThemeColor(DropdownBack),
			.borderColor = GetThemeColor(DropdownBorder),
			.borderThickness = FillV4r(1),
			.floating = {
				.type = UiFloatingType_Id,
				.attachId = btnId,
				.parentSide = UiSide_BottomLeft,
				.elemSide = UiSide_TopLeft,
			},
			//TODO: Add radius to bottomLeft and bottomRight corners
		});
		menuNeedsClosing = true;
	}
	
	return menuNeedsClosing;
}
#define UiTopbarMenuBtn(...) DeferIfBlockCondEnd(UiTopbarMenuBtn_(__VA_ARGS__), CloseUiElement())

bool UiDropdownBtn(UiId btnId, bool isEnabled, AppIcon appIcon, Str8 displayText, Str8 hotkeyStr, Str8 tooltipStr)
{
	PigFont* font = &app->uiFont;
	r32 fontSize = app->uiFontSize;
	u8 fontStyle = UI_FONT_STYLE;
	TextMeasure displayTextSize = MeasureTextEx(font, fontSize, fontStyle, false, 0, displayText);
	
	bool isBtnHovered = IsUiElementHovered(btnId);
	bool isPressed = (isBtnHovered && IsMouseDownRaw(MouseBtn_Left));
	ThemeState btnThemeState = !isEnabled ? ThemeState_Disabled : (isPressed ? ThemeState_Pressed : (isBtnHovered ? ThemeState_Hovered : ThemeState_Default));
	Color32 backgroundColor   = GetThemeColorEx(DropdownBtnBack,   btnThemeState);
	Color32 borderColor       = GetThemeColorEx(DropdownBtnBorder, btnThemeState);
	Color32 textColor         = GetThemeColorEx(DropdownBtnText,   btnThemeState);
	Color32 iconColor         = GetThemeColorEx(DropdownBtnIcon,   btnThemeState);
	Color32 hotkeyBackColor   = GetThemeColorEx(HotkeyBack,        btnThemeState);
	Color32 hotkeyBorderColor = GetThemeColorEx(HotkeyBorder,      btnThemeState);
	Color32 hotkeyTextColor   = GetThemeColorEx(HotkeyText,        btnThemeState);
	
	UIELEM({ .id = btnId,
		.direction = UiLayoutDir_LeftToRight,
		.sizing = { .width=UI_EXPAND(), .height=UI_FIT(), },
		.alignment = UI_ALIGN_LEFT_CENTER(),
		.padding = { .inner=MakeV4r(4,2,4,6) },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1),
	})
	{
		UIELEM_LEAF({
			.text = displayText,
			.font = font,
			.fontSize = fontSize,
			.fontStyle = fontStyle,
			.textColor = textColor,
			.sizing = UI_FIXED2((displayTextSize.Width / app->settings.uiScale), (displayTextSize.Height / app->settings.uiScale)),
		});
	}
	
	return (isBtnHovered && isEnabled && MouseLeftClicked());
}

#endif //BUILD_WITH_PIG_UI