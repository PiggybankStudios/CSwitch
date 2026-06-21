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

void SetScrollbarColors(UiScrollbarState* scrollbarState)
{
	scrollbarState->gutterColor = GetThemeColor(ScrollGutter);
	ThemeState themeState = (scrollbarState->isDragging
		? ThemeState_Pressed
		: (scrollbarState->isHovered
			? ThemeState_Hovered
			: ThemeState_Default)
	);
	scrollbarState->barColor = GetThemeColorEx(ScrollBar, themeState);
}

void UiTopbar_(UiId topbarId)
{
	UiSizingAxis topbarHeight = UI_FIT();
	OpenUiElement(NEW_STRUCT(UiElemConfig){ .id = topbarId,
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
}
#define UiTopbar(...) DeferBlockWithStart(UiTopbar_(__VA_ARGS__), CloseUiElement())

bool UiTopbarMenuBtn_(UiId btnId, Str8 displayText, bool showAltText, bool* isMenuOpen, bool* keepOpenUntilMouseover, bool isSubmenuOpen)
{
	UiId menuId = UiIdSuffixLit(btnId, "_Menu");
	
	Assert(displayText.length >= 1);
	Str8 altDisplayStr = PrintInArenaStr(uiArena, "(%c)%.*s", displayText.chars[0], displayText.length-1, &displayText.chars[1]);
	v2 displayTextSize = MeasureTextEx(&app->uiFont, app->uiFontSize, UI_FONT_STYLE, false, 0.0f, displayText).logicalRec.Size;
	v2 altDisplayStrSize = MeasureTextEx(&app->uiFont, app->uiFontSize, UI_FONT_STYLE, false, 0.0f, altDisplayStr).logicalRec.Size;
	
	bool isBtnHovered = IsUiElementHovered(btnId);
	bool isMenuHovered = IsUiElementHovered(menuId);
	bool isBtnOrMenuHovered = (isBtnHovered || isMenuHovered);
	ThemeState btnThemeState = *isMenuOpen ? ThemeState_Open : (isBtnHovered ? ThemeState_Hovered : ThemeState_Default);
	Color32 backgroundColor = GetThemeColorEx(TopbarBtnBack, btnThemeState);
	Color32 borderColor = GetThemeColorEx(TopbarBtnBorder, btnThemeState);
	Color32 textColor = GetThemeColorEx(TopbarBtnText, btnThemeState);
	
	UIELEM({ .id = btnId,
		.sizing = UI_FIT2(),
		.alignment = UI_ALIGN_CENTER(),
		.padding = { .inner = MakeV4r(4, 2, 4, 2) },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1.0f),
		.cornerRadius = FillV4r(4),
	})
	{
		UIELEM_LEAF({
			.text = showAltText ? altDisplayStr : displayText,
			.alignment = UI_ALIGN_CENTER(),
			.font = &app->uiFont,
			.fontSize = app->uiFontSize,
			.fontStyle = UI_FONT_STYLE,
			.textColor = textColor,
			.sizing = { .width=UI_FIXED(MaxR32(displayTextSize.Width, altDisplayStrSize.Width)), .height={.type=UiSizingType_TextClip} },
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
			.cornerRadius = { .BottomLeft=4, .BottomRight=4 },
		});
		menuNeedsClosing = true;
	}
	
	return menuNeedsClosing;
}
#define UiTopbarMenuBtn(...) DeferIfBlockCondEnd(UiTopbarMenuBtn_(__VA_ARGS__), CloseUiElement())

bool UiDropdownBtn(UiId btnId, bool isEnabled, AppIcon appIcon, Str8 displayText, AppCommand commandForHotkeyDisplay, Str8 tooltipStr)
{
	UNUSED(tooltipStr); //TODO: Add tooltip support!
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
		.padding = { .inner=MakeV4r(4,2,4,6), .child=TOPBAR_ICONS_PADDING },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1.0f),
		.cornerRadius = FillV4r(4),
	})
	{
		if (appIcon != AppIcon_None)
		{
			UIELEM_LEAF({
				.sizing = UI_FIXED2(TOPBAR_ICONS_SIZE, TOPBAR_ICONS_SIZE),
				.texture = &app->appIconsSheet.texture,
				.textureSourceRec = GetSheetCellRec(&app->appIconsSheet, app->appIconSheetCell[appIcon]),
				.color = iconColor,
			});
		}
		
		UIELEM_LEAF({
			.text = displayText,
			.font = &app->uiFont,
			.fontSize = app->uiFontSize,
			.fontStyle = UI_FONT_STYLE,
			.textColor = textColor,
			.sizing = UI_TEXT_FULL(),
		});
		
		if (commandForHotkeyDisplay != AppCommand_None)
		{
			UIEXPANDER_HORI();
			
			Str8 hotkeyText = GetBindingStrForAppCommand(&app->bindings, commandForHotkeyDisplay, uiArena, 0);
			if (!IsEmptyStr(hotkeyText))
			{
				UIELEM({
					.sizing = UI_FIT2(),
					.padding = { .inner = { .Left=2, .Right=2, .Top=1, .Bottom=1 } },
					.color = hotkeyBackColor,
					.borderColor = hotkeyBorderColor,
					.borderThickness = FillV4r(1),
					.cornerRadius = FillV4r(5),
				})
				{
					UIELEM_LEAF({
						.text = hotkeyText,
						.font = &app->uiFont,
						.fontSize = app->uiFontSize,
						.fontStyle = UI_FONT_STYLE,
						.textColor = hotkeyTextColor,
						.sizing = UI_TEXT_FULL(),
					});
				}
			}
		}
	}
	
	return (isBtnHovered && isEnabled && MouseLeftClicked());
}

bool UiDropdownSubmenuBtn_(UiId btnId, bool isEnabled, AppIcon appIcon, Str8 displayText, bool* isMenuOpen, bool* keepOpenUntilMouseoverPntr)
{
	NotNull(isMenuOpen);
	NotNull(keepOpenUntilMouseoverPntr);
	if (*isMenuOpen && !isEnabled) { *isMenuOpen = false; *keepOpenUntilMouseoverPntr = false; WriteLine_D("Disabled, on longer keep over"); }
	
	UiId submenuId = UiIdSuffixLit(btnId, "_Submenu");
	bool isBtnHovered = IsUiElementHovered(btnId);
	bool isMenuHovered = IsUiElementHovered(submenuId);
	bool isPressed = (isBtnHovered && IsMouseDownRaw(MouseBtn_Left));
	
	if (isBtnHovered && isEnabled && MouseLeftClicked())
	{
		*isMenuOpen = !*isMenuOpen;
		if (*isMenuOpen) { *keepOpenUntilMouseoverPntr = true; }
		else { *keepOpenUntilMouseoverPntr = false; }
	}
	if (isMenuHovered && *keepOpenUntilMouseoverPntr) { *keepOpenUntilMouseoverPntr = false; }
	if (*isMenuOpen && !isBtnHovered && !isMenuHovered && !(*keepOpenUntilMouseoverPntr)) { *isMenuOpen = false; }
	
	ThemeState btnThemeState = !isEnabled ? ThemeState_Disabled : (isPressed ? ThemeState_Pressed : (isBtnHovered ? ThemeState_Hovered : ThemeState_Default));
	Color32 backgroundColor   = GetThemeColorEx(DropdownBtnBack,   btnThemeState);
	Color32 borderColor       = GetThemeColorEx(DropdownBtnBorder, btnThemeState);
	Color32 textColor         = GetThemeColorEx(DropdownBtnText,   btnThemeState);
	Color32 iconColor         = GetThemeColorEx(DropdownBtnIcon,   btnThemeState);
	
	UIELEM({ .id = btnId,
		.direction = UiLayoutDir_LeftToRight,
		.sizing = { .width=UI_EXPAND(), .height=UI_FIT(), },
		.alignment = UI_ALIGN_LEFT_CENTER(),
		.padding = { .inner=MakeV4r(4,2,4,6), .child=TOPBAR_ICONS_PADDING },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1.0f),
		.cornerRadius = FillV4r(4),
	})
	{
		if (appIcon != AppIcon_None)
		{
			UIELEM_LEAF({
				.sizing = UI_FIXED2(TOPBAR_ICONS_SIZE, TOPBAR_ICONS_SIZE),
				.texture = &app->appIconsSheet.texture,
				.textureSourceRec = GetSheetCellRec(&app->appIconsSheet, app->appIconSheetCell[appIcon]),
				.color = iconColor,
			});
		}
		
		UIELEM_LEAF({
			.text = displayText,
			.font = &app->uiFont,
			.fontSize = app->uiFontSize,
			.fontStyle = UI_FONT_STYLE,
			.textColor = textColor,
			.sizing = UI_TEXT_FULL(),
		});
	}
	
	if (*isMenuOpen)
	{
		OpenUiElement(NEW_STRUCT(UiElemConfig){ .id = submenuId,
			.direction = UiLayoutDir_TopDown,
			.sizing = UI_FIT2(),
			.padding = { .inner=FillV4r(2), .child=2 },
			.color = GetThemeColor(DropdownBack),
			.borderColor = GetThemeColor(DropdownBorder),
			.borderThickness = FillV4r(1),
			.cornerRadius = MakeV4r(0, 0, 4, 4),
			.floating = {
				.type = UiFloatingType_Id,
				.attachId = btnId,
				.parentSide = UiSide_TopRight,
				.elemSide = UiSide_TopLeft,
			},
		});
		return true;
	}
	else { return false; }
}
#define UiDropdownSubmenuBtn(...) DeferIfBlockCondEnd(UiDropdownSubmenuBtn_(__VA_ARGS__), CloseUiElement())

bool UiOptionBtn(UiId btnId, Str8 nameStr, Str8 valueStr, bool enabled, bool isSelected)
{
	bool isHovered = IsUiElementHovered(btnId);
	bool isPressed = (isHovered && IsMouseDownRaw(MouseBtn_Left));
	
	ThemeState btnThemeState = isPressed ? ThemeState_Pressed : (isSelected ? ThemeState_Selected : (isHovered ? ThemeState_Hovered : ThemeState_Default));
	Color32 backgroundColor = enabled ? GetThemeColorEx(OptionOnBack,      btnThemeState) : GetThemeColorEx(OptionOffBack,      btnThemeState);
	Color32 borderColor     = enabled ? GetThemeColorEx(OptionOnBorder,    btnThemeState) : GetThemeColorEx(OptionOffBorder,    btnThemeState);
	Color32 nameTextColor   = enabled ? GetThemeColorEx(OptionOnNameText,  btnThemeState) : GetThemeColorEx(OptionOffNameText,  btnThemeState);
	Color32 valueTextColor  = enabled ? GetThemeColorEx(OptionOnValueText, btnThemeState) : GetThemeColorEx(OptionOffValueText, btnThemeState);
	
	UIELEM({ .id = btnId,
		.direction = UiLayoutDir_LeftToRight,
		.sizing = { .width=UI_EXPAND(), .height=UI_FIT() },
		.padding = { .inner=FillV4r(4) },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(2),
		.cornerRadius = FillV4r(4),
	})
	{
		//TODO: Determine the clipping style (left vs right)
		UIELEM_LEAF({ .id = UiIdSuffixLit(btnId, "_Name"),
			.sizing = UI_TEXT_FULL(),
			.text = nameStr,
			.textColor = nameTextColor,
			.font = &app->mainFont,
			.fontSize = app->mainFontSize,
			.fontStyle = MAIN_FONT_STYLE,
			.renderer = { .textContraction = (app->settings.clipNamesLeft ? TextContraction_EllipseLeft : TextContraction_EllipseRight) },
		});
		UIEXPANDER_HORI();
		UIELEM_LEAF({ .id = UiIdSuffixLit(btnId, "_Value"),
			.sizing = UI_TEXT_FULL(),
			.text = valueStr,
			.textColor = valueTextColor,
			.font = &app->mainFont,
			.fontSize = app->mainFontSize,
			.fontStyle = MAIN_FONT_STYLE,
		});
	}
	
	return (isHovered && MouseLeftClicked());
}

bool UiSmallOptionBtn(UiId btnId, Str8 abbrevStr, bool enabled, bool isSelected, r32 buttonWidth)
{
	bool isHovered = IsUiElementHovered(btnId);
	bool isPressed = (isHovered && IsMouseDownRaw(MouseBtn_Left));

	ThemeState btnThemeState = isPressed ? ThemeState_Pressed : (isSelected ? ThemeState_Selected : (isHovered ? ThemeState_Hovered : ThemeState_Default));
	Color32 backgroundColor = enabled ? GetThemeColorEx(OptionOnBack,      btnThemeState) : GetThemeColorEx(OptionOffBack,      btnThemeState);
	Color32 borderColor     = enabled ? GetThemeColorEx(OptionOnBorder,    btnThemeState) : GetThemeColorEx(OptionOffBorder,    btnThemeState);
	Color32 nameTextColor   = enabled ? GetThemeColorEx(OptionOnNameText,  btnThemeState) : GetThemeColorEx(OptionOffNameText,  btnThemeState);
	
	UIELEM({ .id = btnId,
		.direction = UiLayoutDir_LeftToRight,
		.alignment = UI_ALIGN_CENTER(),
		.sizing = { .width=UI_FIXED(buttonWidth), .height=UI_FIT() },
		.padding = { .inner = MakeV4r(SMALL_BTN_PADDING_X, SMALL_BTN_PADDING_Y, SMALL_BTN_PADDING_X, SMALL_BTN_PADDING_Y) },
		.color = backgroundColor,
		.cornerRadius = FillV4r(4),
		.borderColor = borderColor,
		.borderThickness = FillV4r(2),
	})
	{
		UIELEM_LEAF({ .id = UiIdSuffixLit(btnId, "_Name"),
			.sizing = UI_TEXT_FULL(),
			.text = abbrevStr,
			.textColor = nameTextColor,
			.font = &app->mainFont,
			.fontSize = app->mainFontSize,
			.fontStyle = MAIN_FONT_STYLE,
		});
	}
	
	return (isHovered && MouseLeftClicked());
}

#endif //BUILD_WITH_PIG_UI