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
	
	bool isBtnHovered = IsUiElementHovered(btnId);
	bool isMenuHovered = IsUiElementHovered(menuId);
	bool isBtnOrMenuHovered = (isBtnHovered || isMenuHovered);
	ThemeState btnThemeState = *isMenuOpen ? ThemeState_Open : (isBtnHovered ? ThemeState_Hovered : ThemeState_Default);
	Color32 backgroundColor = GetThemeColorEx(TopbarBtnBack, btnThemeState);
	Color32 borderColor = GetThemeColorEx(TopbarBtnBorder, btnThemeState);
	Color32 textColor = GetThemeColorEx(TopbarBtnText, btnThemeState);
	
	UIELEM({ .id = btnId,
		.sizing = UI_FIT2(),
		.padding = { .inner = MakeV4r(4, 2, 4, 2) },
		.color = backgroundColor,
		.borderColor = borderColor,
		.borderThickness = FillV4r(1),
		.cornerRadius = FillV4r(4),
	})
	{
		UIELEM_LEAF({
			.text = displayText,
			.font = &app->uiFont,
			.fontSize = app->uiFontSize,
			.fontStyle = UI_FONT_STYLE,
			.textColor = textColor,
			.sizing = UI_TEXT_FULL(),
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
		.borderThickness = FillV4r(1),
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
			UIELEM_LEAF({});
			
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
						//TODO: Add corner radius
						.sizing = UI_TEXT_FULL(),
					});
				}
			}
		}
	}
	
	return (isBtnHovered && isEnabled && MouseLeftClicked());
}

#endif //BUILD_WITH_PIG_UI