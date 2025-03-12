/*
File:   app_clay.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** Holds various functions that comprise common widgets for Clay
*/

//Call Clay__CloseElement once if false, three times if true (i.e. twice inside the if statement and once after)
bool ClayTopBtn(const char* btnText, bool showAltText, bool* isOpenPntr, bool* keepOpenUntilMouseoverPntr, bool keepOpen)
{
	ScratchBegin(scratch);
	ScratchBegin1(persistScratch, scratch);
	
	Str8 normalDisplayStr = StrLit(btnText);
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
	bool isBtnHoveredOrMenuOpen = (isBtnHovered || *isOpenPntr);
	Color32 backgroundColor = isBtnHoveredOrMenuOpen ? HOVERED_BLUE : Transparent;
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWidth = isHovered ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .padding = { UI_U16(4), UI_U16(4), UI_U16(2), UI_U16(2) } },
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(borderWidth)), .color=ToClayColor(borderColor) },
	});
	CLAY({
		.layout = {
			.sizing = { .width=CLAY_SIZING_FIXED(altDisplayStrSize.Width), .height=CLAY_SIZING_FIT(0) },
			.padding = { .left = leftPadding },
		},
	})
	{
		CLAY_TEXT(
			ToClayString(showAltText ? altDisplayStr : normalDisplayStr),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = ToClayColor(TEXT_WHITE),
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_CENTER,
			})
		);
	}
	if (IsMouseOverClay(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left)) { *isOpenPntr = !*isOpenPntr; }
	if (*isOpenPntr == true && isHovered && *keepOpenUntilMouseoverPntr) { *keepOpenUntilMouseoverPntr = false; } //once we are closed or the mouse is over, clear this flag, mouse leaving now will constitute closing
	if (*isOpenPntr == true && !isHovered && !*keepOpenUntilMouseoverPntr && !keepOpen) { *isOpenPntr = false; }
	if (*isOpenPntr)
	{
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuId,
			.floating = {
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.attachPoints = {
					.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM,
				},
			},
			.layout = {
				.padding = { 2, 2, 0, 0 },
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
			.backgroundColor = ToClayColor(BACKGROUND_GRAY),
			.border = { .color=ToClayColor(OUTLINE_GRAY), .width={ .bottom=1 } },
			.cornerRadius = { 0, 0, 4, 4 },
		});
	}
	//NOTE: We do NOT do ScratchEnd on persistScratch, the string needs to live to the end of the frame where the UI will get rendered
	ScratchEnd(scratch);
	return *isOpenPntr;
}

#define CLAY_ICON(texturePntr, size, color) CLAY({      \
	.layout = {                                         \
		.sizing = {                                     \
			.width = CLAY_SIZING_FIXED((size).Width),   \
			.height = CLAY_SIZING_FIXED((size).Height), \
		},                                              \
	},                                                  \
	.image = {                                          \
		.imageData = (texturePntr),                     \
		.sourceDimensions = {                           \
			.width = (r32)((texturePntr)->Width),       \
			.height = (r32)((texturePntr)->Height),     \
		},                                              \
	},                                                  \
	.backgroundColor = ToClayColor(color),              \
}) {}

bool ClayTopSubmenu(const char* btnText, bool isParentOpen, bool* isOpenPntr, Texture* icon)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopSubmenu", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopSubmenuMenu", btnText);
	Str8 menuListIdStr = PrintInArenaStr(scratch, "%s_TopSubmenuMenuList", btnText);
	ClayId btnId = ToClayId(btnIdStr);
	ClayId menuId = ToClayId(menuIdStr);
	ClayId menuListId = ToClayId(menuListIdStr);
	bool isBtnHovered = IsMouseOverClay(btnId);
	bool isHovered = (isBtnHovered || IsMouseOverClay(menuId) || IsMouseOverClay(menuListId));
	bool isBtnHoveredOrMenuOpen = (isBtnHovered || *isOpenPntr);
	Color32 backgroundColor = isBtnHoveredOrMenuOpen ? HOVERED_BLUE : Transparent;
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWidth = isHovered ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .sizing = { .width=CLAY_SIZING_GROW(0), }, .padding = { UI_U16(4), UI_U16(4), UI_U16(16), UI_U16(16) } },
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(borderWidth)), .color=ToClayColor(borderColor) },
	});
	CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = TOPBAR_ICONS_PADDING, .padding = { .right = UI_U16(8) }, } })
	{
		if (icon != nullptr)
		{
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE * app->uiScale), TEXT_WHITE);
		}
		CLAY_TEXT(
			ToClayString(StrLit(btnText)),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = ToClayColor(TEXT_WHITE),
			})
		);
	}
	if (!isParentOpen) { *isOpenPntr = false; }
	if (isParentOpen && IsMouseOverClay(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left)) { *isOpenPntr = !*isOpenPntr; }
	if (*isOpenPntr == true && !isHovered) { *isOpenPntr = false; }
	if (*isOpenPntr)
	{
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuId,
			.floating = {
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.attachPoints = {
					.parent = CLAY_ATTACH_POINT_RIGHT_TOP,
				},
			},
			.layout = {
				.padding = { 0, 0, 0, 0 },
			}
		});
		
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.id = menuListId,
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.padding = CLAY_PADDING_ALL(UI_U16(2)),
				.childGap = 2,
			},
			.backgroundColor = ToClayColor(BACKGROUND_GRAY),
			.border = { .color=ToClayColor(OUTLINE_GRAY), .width={ .bottom=1 } },
			.cornerRadius = { 0, 0, 4, 4 },
		});
	}
	ScratchEnd(scratch);
	return *isOpenPntr;
}

//Call Clay__CloseElement once after if statement
bool ClayBtnStrEx(Str8 idStr, Str8 btnText, Str8 hotkeyStr, bool isEnabled, Texture* icon)
{
	ScratchBegin(scratch);
	Str8 fullIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(idStr));
	Str8 hotkeyIdStr = PrintInArenaStr(scratch, "%.*s_Hotkey", StrPrint(idStr));
	ClayId btnId = ToClayId(fullIdStr);
	ClayId hotkeyId = ToClayId(hotkeyIdStr);
	bool isHovered = IsMouseOverClay(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Color32 backgroundColor = !isEnabled ? BACKGROUND_BLACK : (isPressed ? SELECTED_BLUE : (isHovered ? HOVERED_BLUE : Transparent));
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWidth = (isHovered && isEnabled) ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.padding = { .top = UI_U16(8), .bottom = UI_U16(8), .left = UI_U16(4), .right = UI_U16(4), },
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(borderWidth)), .color=ToClayColor(borderColor) },
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
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE * app->uiScale), TEXT_WHITE);
		}
		CLAY_TEXT(
			ToClayString(btnText),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = (u16)app->uiFontSize,
				.textColor = ToClayColor(TEXT_WHITE),
			})
		);
		if (!IsEmptyStr(hotkeyStr))
		{
			CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, } }) {}
			
			CLAY({ .id=hotkeyId,
				.layout = {
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
					.padding = CLAY_PADDING_ALL(UI_U16(2)),
				},
				.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(1)), .color = ToClayColor(TEXT_GRAY) },
				.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(5)),
			})
			{
				CLAY_TEXT(
					ToClayString(hotkeyStr),
					CLAY_TEXT_CONFIG({
						.fontId = app->clayUiFontId,
						.fontSize = (u16)app->uiFontSize,
						.textColor = ToClayColor(TEXT_GRAY),
					})
				);
			}
		}
	}
	ScratchEnd(scratch);
	return (isHovered && isEnabled && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}
bool ClayBtnStr(Str8 btnText, Str8 hotkeyStr, bool isEnabled, Texture* icon)
{
	return ClayBtnStrEx(btnText, btnText, hotkeyStr, isEnabled, icon);
}
bool ClayBtn(const char* btnText, const char* hotkeyStr, bool isEnabled, Texture* icon)
{
	return ClayBtnStr(StrLit(btnText), StrLit(hotkeyStr), isEnabled, icon);
}

//Call Clay__CloseElement once after if statement
bool ClayOptionBtn(ClayId containerId, Str8 nameStr, Str8 valueStr, bool enabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(nameStr));
	ClayId btnId = ToClayId(btnIdStr);
	bool isHovered = IsMouseOverClayInContainer(containerId, btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Color32 backColor = enabled ? SELECTED_BLUE : Transparent;
	Color32 hoverColor = ColorLerpSimple(HOVERED_BLUE, SELECTED_BLUE, enabled ? 0.75f : 0.0f);
	Color32 pressColor = ColorLerpSimple(HOVERED_BLUE, SELECTED_BLUE, 0.5f);
	Color32 textColor = TEXT_WHITE;
	Color32 valueColor = enabled ? TEXT_WHITE : TEXT_GRAY;
	Color32 outlineColor = enabled ? SELECTED_BLUE : (isHovered ? ColorLerpSimple(HOVERED_BLUE, SELECTED_BLUE, 0.5f) : OUTLINE_GRAY);
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.padding = CLAY_PADDING_ALL(UI_U16(4)),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(isPressed ? pressColor : (isHovered ? hoverColor : backColor)),
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
		.border = {
			.color = ToClayColor(outlineColor),
			.width = CLAY_BORDER_OUTSIDE(UI_BORDER(2)),
		},
	});
	CLAY_TEXT(
		ToClayString(nameStr),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayMainFontId,
			.fontSize = (u16)app->mainFontSize,
			.textColor = ToClayColor(textColor),
			.wrapMode = CLAY_TEXT_WRAP_NONE,
			.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
			.userData = { .contraction = app->clipNamesOnLeftSide ? TextContraction_EllipseLeft : TextContraction_EllipseRight },
		})
	);
	CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
	if (!IsEmptyStr(valueStr))
	{
		CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(UI_U16(4)) } } }) {} //ensure 4px padding between name and value
		CLAY_TEXT(
			ToClayString(valueStr),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayMainFontId,
				.fontSize = (u16)app->mainFontSize,
				.textColor = ToClayColor(valueColor),
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
	Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(scrollContainerId);
	r32 scrollbarYPercent = 0.0f;
	r32 scrollbarSizePercent = 1.0f;
	if (scrollData.found && scrollData.contentDimensions.height > scrollData.scrollContainerDimensions.height)
	{
		scrollbarSizePercent = ClampR32(scrollData.scrollContainerDimensions.height / scrollData.contentDimensions.height, 0.0f, 1.0f);
		scrollbarYPercent = ClampR32(-scrollData.scrollPosition->y / (scrollData.contentDimensions.height - scrollData.scrollContainerDimensions.height), 0.0f, 1.0f);
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
			.backgroundColor = ToClayColor(BACKGROUND_BLACK),
		})
		{
			rec scrollGutterDrawRec = GetClayElementDrawRec(gutterId);
			v2 scrollBarSize = NewV2(
				gutterWidth - (UI_U16(1) * 2.0f),
				scrollGutterDrawRec.Height * scrollbarSizePercent
			);
			r32 scrollBarOffsetY = ClampR32((scrollGutterDrawRec.Height - scrollBarSize.Height) * scrollbarYPercent, 0.0f, scrollGutterDrawRec.Height);
			CLAY({ .id = scrollbarId,
				.floating = {
					.attachTo = CLAY_ATTACH_TO_PARENT,
					.offset = { .x = UI_U16(1), .y = scrollBarOffsetY },
				},
				.layout = {
					.sizing = {
						.width = CLAY_SIZING_FIXED(scrollBarSize.X),
						.height = CLAY_SIZING_FIXED(scrollBarSize.Y),
					},
				},
				.backgroundColor = ToClayColor((isHovered || state->isDragging) ? TEXT_GRAY : OUTLINE_GRAY),
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
					state->grabOffset = NewV2(scrollbarDrawRec.Width/2, scrollbarDrawRec.Height/2);
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
					scrollData.scrollTarget->y = -((scrollData.contentDimensions.height - scrollData.scrollContainerDimensions.height) * newScrollbarPercent);
					if (!state->isDraggingSmooth) { scrollData.scrollPosition->y = scrollData.scrollTarget->y; }
				}
			}
			if (scrollData.scrollPosition->y == scrollData.scrollTarget->y) { state->isDraggingSmooth = false; }
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
