/*
File:   app_clay.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** Holds various functions that comprise common widgets for Clay
*/

//Call Clay__CloseElement once if false, three times if true (i.e. twice inside the if statement and once after)
bool ClayTopBtn(const char* btnText, bool showAltText, bool* isOpenPntr, bool* keepOpenUntilMouseoverPntr, bool isSubmenuOpen)
{
	ScratchBegin(scratch);
	ScratchBegin1(persistScratch, scratch);
	
	Str8 normalDisplayStr = MakeStr8Nt(btnText);
	Assert(!IsEmptyStr(normalDisplayStr));
	Str8 altDisplayStr = PrintInArenaStr(persistScratch, "(%c)%.*s", normalDisplayStr.chars[0], normalDisplayStr.length-1, &normalDisplayStr.chars[1]);
	v2 normalDisplayStrSize = ClayUiTextSize(&app->uiFont, app->uiFontSize, UI_FONT_STYLE, normalDisplayStr);
	v2 altDisplayStrSize = ClayUiTextSize(&app->uiFont, app->uiFontSize, UI_FONT_STYLE, altDisplayStr);
	u16 leftPadding = (u16)(showAltText ? 0 : (altDisplayStrSize.Width - normalDisplayStrSize.Width)/2);
	
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopBtn", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopBtnMenu", btnText);
	ClayId btnId = ToClayId(btnIdStr);
	ClayId menuId = ToClayId(menuIdStr);
	bool isBtnHovered = IsMouseOverClay(btnId);
	bool isHovered = (isBtnHovered || IsMouseOverClay(menuId));
	Color32 backgroundColor = *isOpenPntr ? GetThemeColor(TopbarBtnBackOpen) : (isBtnHovered ? GetThemeColor(TopbarBtnBackHover) : GetThemeColor(TopbarBtnBack));
	Color32 borderColor = *isOpenPntr ? GetThemeColor(TopbarBtnBorderOpen) : (isBtnHovered ? GetThemeColor(TopbarBtnBorderHover) : GetThemeColor(TopbarBtnBorder));
	Color32 textColor = *isOpenPntr ? GetThemeColor(TopbarBtnTextOpen) : (isBtnHovered ? GetThemeColor(TopbarBtnTextHover) : GetThemeColor(TopbarBtnText));
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .padding = { UI_U16(4), UI_U16(4), UI_U16(2), UI_U16(2) } },
		.backgroundColor = backgroundColor,
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(1)), .color=borderColor },
	});
	CLAY({
		.layout = {
			.sizing = { .width=CLAY_SIZING_FIXED(altDisplayStrSize.Width), .height=CLAY_SIZING_FIT(0) },
			.padding = { .left = leftPadding },
		},
	})
	{
		CLAY_TEXT(
			showAltText ? altDisplayStr : normalDisplayStr,
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = textColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_CENTER,
			})
		);
	}
	if (IsMouseOverClay(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left)) { *isOpenPntr = !*isOpenPntr; }
	if (*isOpenPntr == true && isHovered && *keepOpenUntilMouseoverPntr) { *keepOpenUntilMouseoverPntr = false; } //once we are closed or the mouse is over, clear this flag, mouse leaving now will constitute closing
	if (*isOpenPntr == true && !isHovered && !*keepOpenUntilMouseoverPntr && !isSubmenuOpen) { *isOpenPntr = false; }
	if (*isOpenPntr)
	{
		r32 maxDropdownWidth = isSubmenuOpen ? appIn->screenSize.Width/2.0f : appIn->screenSize.Width;
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuId,
			.floating = {
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.zIndex = 5,
				.attachPoints = {
					.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM,
				},
			},
			.layout = {
				.padding = { 2, 2, 0, 0 },
				.sizing = { .width = CLAY_SIZING_FIT(0, maxDropdownWidth) },
			}
		});
		
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.padding = {
					.left = 1,
					.right = 1,
					.top = 2,
					.bottom = 2,
				},
				.childGap = 2,
			},
			.backgroundColor = GetThemeColor(DropdownBack),
			.border = { .color=GetThemeColor(DropdownBorder), .width={ .bottom=1 } }, //TODO: $THEME Should we have a border on all 3 sides?
			.cornerRadius = { 0, 0, 4, 4 },
		});
	}
	//NOTE: We do NOT do ScratchEnd on persistScratch, the string needs to live to the end of the frame where the UI will get rendered
	ScratchEnd(scratch);
	return *isOpenPntr;
}

bool ClayTopSubmenu(const char* btnText, bool isParentOpen, bool* isOpenPntr, bool* keepOpenUntilMouseoverPntr, Texture* icon)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopSubmenu", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopSubmenuMenu", btnText);
	Str8 menuListIdStr = PrintInArenaStr(scratch, "%s_TopSubmenuMenuList", btnText);
	ClayId btnId = ToClayId(btnIdStr);
	ClayId menuId = ToClayId(menuIdStr);
	ClayId menuListId = ToClayId(menuListIdStr);
	bool isBtnHovered = IsMouseOverClay(btnId);
	bool isBtnPressed = (isBtnHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	bool isMenuHovered = (IsMouseOverClay(menuId) || IsMouseOverClay(menuListId));
	bool isHovered = (isBtnHovered || isMenuHovered);
	Color32 backgroundColor =
		  isBtnPressed ? GetThemeColor(DropdownBtnBackPressed)
		: isBtnHovered ? GetThemeColor(DropdownBtnBackHover)
		:  *isOpenPntr ? GetThemeColor(DropdownBtnBackOpen)
		:                GetThemeColor(DropdownBtnBack);
	Color32 borderColor =
		  isBtnPressed ? GetThemeColor(DropdownBtnBorderPressed)
		: isBtnHovered ? GetThemeColor(DropdownBtnBorderHover)
		:  *isOpenPntr ? GetThemeColor(DropdownBtnBorderOpen)
		:                GetThemeColor(DropdownBtnBorder);
	Color32 textColor =
		  isBtnPressed ? GetThemeColor(DropdownBtnTextPressed)
		: isBtnHovered ? GetThemeColor(DropdownBtnTextHover)
		:  *isOpenPntr ? GetThemeColor(DropdownBtnTextOpen)
		:                GetThemeColor(DropdownBtnText);
	Color32 iconColor =
		  isBtnPressed ? GetThemeColor(DropdownBtnIconPressed)
		: isBtnHovered ? GetThemeColor(DropdownBtnIconHover)
		:  *isOpenPntr ? GetThemeColor(DropdownBtnIconOpen)
		:                GetThemeColor(DropdownBtnIcon);
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .sizing = { .width=CLAY_SIZING_GROW(0), }, .padding = { UI_U16(4), UI_U16(4), UI_U16(8), UI_U16(8) } },
		.backgroundColor = backgroundColor,
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(1)), .color=borderColor },
	});
	CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = TOPBAR_ICONS_PADDING, .padding = { .right = UI_U16(8) }, } })
	{
		if (icon != nullptr)
		{
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE * app->uiScale), iconColor);
		}
		CLAY_TEXT(
			MakeStr8Nt(btnText),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = textColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
				.userData = { .contraction = TextContraction_ClipRight },
			})
		);
	}
	if (!isParentOpen) { *isOpenPntr = false; *keepOpenUntilMouseoverPntr = false; }
	if (isParentOpen && IsMouseOverClay(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left))
	{
		*isOpenPntr = !*isOpenPntr;
		*keepOpenUntilMouseoverPntr = *isOpenPntr;
	}
	if (*isOpenPntr == true && isMenuHovered && *keepOpenUntilMouseoverPntr) { *keepOpenUntilMouseoverPntr = false; } //once we are closed or the mouse is over, clear this flag, mouse leaving now will constitute closing
	if (*isOpenPntr == true && !isHovered && !*keepOpenUntilMouseoverPntr) { *isOpenPntr = false; *keepOpenUntilMouseoverPntr = false; }
	if (*isOpenPntr)
	{
		r32 maxDropdownWidth = appIn->screenSize.Width/2.0f;
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuId,
			.floating = {
				.zIndex = 6,
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.attachPoints = {
					.parent = CLAY_ATTACH_POINT_RIGHT_TOP,
				},
			},
			.layout = {
				.padding = { 0, 0, 0, 0 },
				.sizing = { .width = CLAY_SIZING_FIT(0, maxDropdownWidth) },
			}
		});
		
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuListId,
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.padding = CLAY_PADDING_ALL(UI_U16(2)),
				.childGap = 2, //TOOD: Convert this to use UI_ macros!
			},
			.backgroundColor = GetThemeColor(DropdownBack),
			.border = { .color=GetThemeColor(DropdownBorder), .width={ .bottom=1 } }, //TODO: $THEME Should we have a border on all 3 sides? TODO: Convert this to use UI_ macros!
			.cornerRadius = { 0, 0, 4, 4 }, //TOOD: Convert this to use UI_ macros!
		});
	}
	ScratchEnd(scratch);
	return *isOpenPntr;
}

//Call Clay__CloseElement once after if statement
bool ClayBtnStrEx(Str8 idStr, Str8 btnText, Str8 hotkeyStr, Str8 tooltipStr, bool isEnabled, Texture* icon)
{
	ScratchBegin(scratch);
	Str8 fullIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(idStr));
	Str8 hotkeyIdStr = PrintInArenaStr(scratch, "%.*s_Hotkey", StrPrint(idStr));
	ClayId btnId = ToClayId(fullIdStr);
	ClayId hotkeyId = ToClayId(hotkeyIdStr);
	bool isHovered = IsMouseOverClay(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	
	Color32 backgroundColor =
		  !isEnabled  ? GetThemeColor(DropdownBtnBackDisabled)
		: isPressed   ? GetThemeColor(DropdownBtnBackPressed)
		: isHovered   ? GetThemeColor(DropdownBtnBackHover)
		:               GetThemeColor(DropdownBtnBack);
	Color32 borderColor =
		  !isEnabled  ? GetThemeColor(DropdownBtnBorderDisabled)
		: isPressed   ? GetThemeColor(DropdownBtnBorderPressed)
		: isHovered   ? GetThemeColor(DropdownBtnBorderHover)
		:               GetThemeColor(DropdownBtnBorder);
	Color32 textColor =
		  !isEnabled  ? GetThemeColor(DropdownBtnTextDisabled)
		: isPressed   ? GetThemeColor(DropdownBtnTextPressed)
		: isHovered   ? GetThemeColor(DropdownBtnTextHover)
		:               GetThemeColor(DropdownBtnText);
	Color32 iconColor =
		  !isEnabled  ? GetThemeColor(DropdownBtnIconDisabled)
		: isPressed   ? GetThemeColor(DropdownBtnIconPressed)
		: isHovered   ? GetThemeColor(DropdownBtnIconHover)
		:               GetThemeColor(DropdownBtnIcon);
	Color32 hotkeyBackColor =
		  !isEnabled  ? GetThemeColor(HotkeyBackDisabled)
		: isPressed   ? GetThemeColor(HotkeyBackPressed)
		: isHovered   ? GetThemeColor(HotkeyBackHover)
		:               GetThemeColor(HotkeyBack);
	Color32 hotkeyBorderColor =
		  !isEnabled  ? GetThemeColor(HotkeyBorderDisabled)
		: isPressed   ? GetThemeColor(HotkeyBorderPressed)
		: isHovered   ? GetThemeColor(HotkeyBorderHover)
		:               GetThemeColor(HotkeyBorder);
	Color32 hotkeyTextColor =
		  !isEnabled  ? GetThemeColor(HotkeyTextDisabled)
		: isPressed   ? GetThemeColor(HotkeyTextPressed)
		: isHovered   ? GetThemeColor(HotkeyTextHover)
		:               GetThemeColor(HotkeyText);
	
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.padding = { .top = UI_U16(3), .bottom = UI_U16(3), .left = UI_U16(4), .right = UI_U16(4), },
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = backgroundColor,
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(1)), .color=borderColor },
		.tooltip = {
			.text = tooltipStr,
			.fontId = app->clayUiFontId,
			.fontSize = (u16)app->uiFontSize,
		},
	});
	CLAY({
		.layout = {
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.childGap = TOPBAR_ICONS_PADDING,
			.sizing = { .width = CLAY_SIZING_GROW(0) },
			.padding = { .right = 0 },
		},
	})
	{
		if (icon != nullptr)
		{
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE * app->uiScale), iconColor);
		}
		CLAY_TEXT(
			btnText,
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = textColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
				.userData = { .contraction = TextContraction_ClipRight },
			})
		);
		if (!IsEmptyStr(hotkeyStr))
		{
			CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, } }) {}
			
			CLAY({ .id=hotkeyId,
				.layout = {
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
					.padding = { .left=UI_U16(2), .right=UI_U16(2), .top=UI_U16(1), .bottom=UI_U16(1) },
				},
				.backgroundColor = hotkeyBackColor,
				.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(1)), .color = hotkeyBorderColor },
				.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(5)),
			})
			{
				CLAY_TEXT(
					hotkeyStr,
					CLAY_TEXT_CONFIG({
						.fontId = app->clayUiFontId,
						.fontSize = (u16)app->uiFontSize,
						.textColor = hotkeyTextColor,
						.wrapMode = CLAY_TEXT_WRAP_NONE,
						.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
						.userData = { .contraction = TextContraction_ClipRight },
					})
				);
			}
		}
	}
	ScratchEnd(scratch);
	return (isHovered && isEnabled && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}
bool ClayBtnStr(Str8 btnText, Str8 hotkeyStr, Str8 tooltipStr, bool isEnabled, Texture* icon)
{
	return ClayBtnStrEx(btnText, btnText, hotkeyStr, tooltipStr, isEnabled, icon);
}
bool ClayBtn(const char* btnText, const char* hotkeyStr, const char* tooltipStr, bool isEnabled, Texture* icon)
{
	return ClayBtnStr(MakeStr8Nt(btnText), MakeStr8Nt(hotkeyStr), MakeStr8Nt(tooltipStr), isEnabled, icon);
}

//Call Clay__CloseElement once after if statement
bool ClayOptionBtn(ClayId containerId, Str8 idStr, Str8 nameStr, Str8 valueStr, bool enabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(idStr));
	ClayId btnId = ToClayId(btnIdStr);
	bool isHovered = IsMouseOverClayInContainer(containerId, btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	
	Color32 backgroundColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionBackTurningOn)
			: isHovered ? GetThemeColor(OptionBackOnHovered)
			:             GetThemeColor(OptionBackOn)
		) : (
			  isPressed ? GetThemeColor(OptionBackTurningOff)
			: isHovered ? GetThemeColor(OptionBackOffHovered)
			:             GetThemeColor(OptionBackOff)
		)
	);
	Color32 borderColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionBorderTurningOn)
			: isHovered ? GetThemeColor(OptionBorderOnHovered)
			:             GetThemeColor(OptionBorderOn)
		) : (
			  isPressed ? GetThemeColor(OptionBorderTurningOff)
			: isHovered ? GetThemeColor(OptionBorderOffHovered)
			:             GetThemeColor(OptionBorderOff)
		)
	);
	Color32 nameTextColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionNameTextTurningOn)
			: isHovered ? GetThemeColor(OptionNameTextOnHovered)
			:             GetThemeColor(OptionNameTextOn)
		) : (
			  isPressed ? GetThemeColor(OptionNameTextTurningOff)
			: isHovered ? GetThemeColor(OptionNameTextOffHovered)
			:             GetThemeColor(OptionNameTextOff)
		)
	);
	Color32 valueTextColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionValueTextTurningOn)
			: isHovered ? GetThemeColor(OptionValueTextOnHovered)
			:             GetThemeColor(OptionValueTextOn)
		) : (
			  isPressed ? GetThemeColor(OptionValueTextTurningOff)
			: isHovered ? GetThemeColor(OptionValueTextOffHovered)
			:             GetThemeColor(OptionValueTextOff)
		)
	);
	
	CLAY({ .id = btnId,
		.layout = {
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.padding = CLAY_PADDING_ALL(UI_U16(4)),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = backgroundColor,
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = {
			.color = borderColor,
			.width = CLAY_BORDER_OUTSIDE(UI_BORDER(2)),
		},
		.tooltip = {
			.text = nameStr,
			.fontId = app->clayUiFontId,
			.fontSize = (u16)app->uiFontSize,
			.inactive = app->settings.optionTooltipsDisabled,
			.containerId = containerId,
		},
	})
	{
		CLAY_TEXT(
			nameStr,
			CLAY_TEXT_CONFIG({
				.fontId = app->clayMainFontId,
				.fontSize = (u16)app->mainFontSize,
				.textColor = nameTextColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
				.userData = { .contraction = app->settings.clipNamesLeft ? TextContraction_EllipseLeft : TextContraction_EllipseRight },
			})
		);
		CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
		if (!IsEmptyStr(valueStr))
		{
			CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(UI_U16(4)) } } }) {} //ensure 4px padding between name and value
			CLAY_TEXT(
				valueStr,
				CLAY_TEXT_CONFIG({
					.fontId = app->clayMainFontId,
					.fontSize = (u16)app->mainFontSize,
					.textColor = valueTextColor,
				})
			);
		}
	}
	ScratchEnd(scratch);
	return (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}

bool ClaySmallOptionBtn(ClayId containerId, r32 buttonWidth, Str8 idStr, Str8 abbreviation, bool enabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(idStr));
	ClayId btnId = ToClayId(btnIdStr);
	bool isHovered = IsMouseOverClayInContainer(containerId, btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	
	Color32 backgroundColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionBackTurningOn)
			: isHovered ? GetThemeColor(OptionBackOnHovered)
			:             GetThemeColor(OptionBackOn)
		) : (
			  isPressed ? GetThemeColor(OptionBackTurningOff)
			: isHovered ? GetThemeColor(OptionBackOffHovered)
			:             GetThemeColor(OptionBackOff)
		)
	);
	Color32 borderColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionBorderTurningOn)
			: isHovered ? GetThemeColor(OptionBorderOnHovered)
			:             GetThemeColor(OptionBorderOn)
		) : (
			  isPressed ? GetThemeColor(OptionBorderTurningOff)
			: isHovered ? GetThemeColor(OptionBorderOffHovered)
			:             GetThemeColor(OptionBorderOff)
		)
	);
	Color32 nameTextColor = (enabled
		? (
			  isPressed ? GetThemeColor(OptionNameTextTurningOn)
			: isHovered ? GetThemeColor(OptionNameTextOnHovered)
			:             GetThemeColor(OptionNameTextOn)
		) : (
			  isPressed ? GetThemeColor(OptionNameTextTurningOff)
			: isHovered ? GetThemeColor(OptionNameTextOffHovered)
			:             GetThemeColor(OptionNameTextOff)
		)
	);
	
	u16 buttonPaddingX = UI_U16(SMALL_BTN_PADDING_X);
	u16 buttonPaddingY = UI_U16(SMALL_BTN_PADDING_Y);
	CLAY({ .id = btnId,
		.layout = {
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.padding = { buttonPaddingX, buttonPaddingX, buttonPaddingY, buttonPaddingY },
			.sizing = { .width = CLAY_SIZING_FIXED(buttonWidth) },
			.childAlignment = { .x = CLAY_ALIGN_X_CENTER },
		},
		.backgroundColor = backgroundColor,
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = {
			.color = borderColor,
			.width = CLAY_BORDER_OUTSIDE(UI_BORDER(2)),
		},
		.tooltip = {
			.text = idStr,
			.fontId = app->clayUiFontId,
			.fontSize = (u16)app->uiFontSize,
			.inactive = app->settings.optionTooltipsDisabled,
			.containerId = containerId,
		},
	})
	{
		CLAY_TEXT(
			abbreviation,
			CLAY_TEXT_CONFIG({
				.fontId = app->clayMainFontId,
				.fontSize = (u16)app->mainFontSize,
				.textColor = nameTextColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_CENTER,
			})
		);
	}
	ScratchEnd(scratch);
	return (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}

// Returns whether the scrollbar is currently displayed
bool ClayScrollbar(ClayId scrollContainerId, Str8 scrollbarIdStr, r32 gutterWidth, ScrollbarInteractionState* state)
{
	NotNull(state);
	ScratchBegin(scratch);
	
	Str8 gutterIdStr = PrintInArenaStr(scratch, "%.*s_Gutter", StrPrint(scrollbarIdStr));
	ClayId gutterId = ToClayId(gutterIdStr);
	ClayId scrollbarId = ToClayId(scrollbarIdStr);
	Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(scrollContainerId, false);
	r32 scrollbarYPercent = 0.0f;
	r32 scrollbarSizePercent = 1.0f;
	if (scrollData.found && scrollData.contentDimensions.Height > scrollData.scrollContainerDimensions.Height)
	{
		scrollbarSizePercent = ClampR32(scrollData.scrollContainerDimensions.Height / scrollData.contentDimensions.Height, 0.0f, 1.0f);
		scrollbarYPercent = ClampR32(-scrollData.scrollPosition->Y / (scrollData.contentDimensions.Height - scrollData.scrollContainerDimensions.Height), 0.0f, 1.0f);
	}
	
	bool isScrollbarVisible = (scrollData.found && scrollbarSizePercent < 1.0f);
	bool isHovered = IsMouseOverClay(scrollbarId);
	
	if (isScrollbarVisible)
	{
		CLAY({ .id = gutterId,
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.padding = { .left = UI_U16(1), },
				.sizing = {
					.width = CLAY_SIZING_FIXED(gutterWidth),
					.height = CLAY_SIZING_GROW(0)
				},
			},
			.backgroundColor = GetThemeColor(ScrollGutter),
		})
		{
			rec scrollGutterDrawRec = GetClayElementDrawRec(gutterId);
			v2 scrollBarSize = MakeV2(
				gutterWidth - (UI_U16(1) * 2.0f),
				scrollGutterDrawRec.Height * scrollbarSizePercent
			);
			r32 scrollBarOffsetY = ClampR32((scrollGutterDrawRec.Height - scrollBarSize.Height) * scrollbarYPercent, 0.0f, scrollGutterDrawRec.Height);
			Color32 scrollBarColor =
				  state->isDragging ? GetThemeColor(ScrollBarGrab)
				: isHovered ?         GetThemeColor(ScrollBarHover)
				:                     GetThemeColor(ScrollBar);
			
			CLAY({ .id = scrollbarId,
				.floating = {
					.attachTo = CLAY_ATTACH_TO_PARENT,
					.zIndex = 1,
					.offset = MakeV2(UI_R32(1), scrollBarOffsetY),
				},
				.layout = {
					.sizing = {
						.width = CLAY_SIZING_FIXED(scrollBarSize.X),
						.height = CLAY_SIZING_FIXED(scrollBarSize.Y),
					},
				},
				.backgroundColor = scrollBarColor,
				.cornerRadius = CLAY_CORNER_RADIUS(scrollBarSize.Width/2),
			}) {}
		}
		
		if (!state->isDragging)
		{
			if (IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left))
			{
				if (IsMouseOverClay(scrollbarId))
				{
					rec scrollbarDrawRec = GetClayElementDrawRec(scrollbarId);
					state->isDragging = true;
					state->isDraggingSmooth = false;
					state->grabOffset = Sub(appIn->mouse.position, scrollbarDrawRec.TopLeft);
				}
				else if (IsMouseOverClay(gutterId))
				{
					rec scrollbarDrawRec = GetClayElementDrawRec(scrollbarId);
					state->isDragging = true;
					state->isDraggingSmooth = true;
					state->grabOffset = MakeV2(scrollbarDrawRec.Width/2, scrollbarDrawRec.Height/2);
				}
			}
		}
		
		if (state->isDragging)
		{
			if (!IsMouseBtnDown(&appIn->mouse, MouseBtn_Left))
			{
				state->isDragging = false;
			}
			else
			{
				rec scrollGutterDrawRec = GetClayElementDrawRec(gutterId);
				rec scrollbarDrawRec = GetClayElementDrawRec(scrollbarId);
				r32 minY = scrollGutterDrawRec.Y;
				r32 maxY = scrollGutterDrawRec.Y + scrollGutterDrawRec.Height - scrollbarDrawRec.Height;
				if (maxY > minY)
				{
					r32 newScrollbarPos = ClampR32(appIn->mouse.position.Y - state->grabOffset.Y, minY, maxY);
					r32 newScrollbarPercent = (newScrollbarPos - minY) / (maxY - minY);
					scrollData.scrollTarget->Y = -((scrollData.contentDimensions.Height - scrollData.scrollContainerDimensions.Height) * newScrollbarPercent);
					if (!state->isDraggingSmooth) { scrollData.scrollPosition->Y = scrollData.scrollTarget->Y; }
				}
			}
			if (scrollData.scrollPosition->Y == scrollData.scrollTarget->Y) { state->isDraggingSmooth = false; }
		}
	}
	else if (state->isDragging)
	{
		state->isDragging = false;
		state->grabOffset = V2_Zero;
	}
	
	ScratchEnd(scratch);
	return isScrollbarVisible;
}
