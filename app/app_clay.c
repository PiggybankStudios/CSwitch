/*
File:   app_clay.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** Holds various functions that comprise common widgets for Clay
*/

bool IsMouseOverClay(ClayId clayId)
{
	return appIn->mouse.isOverWindow && Clay_PointerOver(clayId);
}

//Call Clay__CloseElement once if false, three times if true (i.e. twice inside the if statement and once after)
bool ClayTopBtn(const char* btnText, bool* isOpenPntr, bool keepOpen)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopBtn", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopBtnMenu", btnText);
	ClayId btnId = ToClayId(btnIdStr);
	ClayId menuId = ToClayId(menuIdStr);
	bool isBtnHovered = IsMouseOverClay(btnId);
	bool isHovered = (isBtnHovered || IsMouseOverClay(menuId));
	bool isBtnHoveredOrMenuOpen = (isBtnHovered || *isOpenPntr);
	Color32 backgroundColor = isBtnHoveredOrMenuOpen ? HOVERED_BLUE : Transparent;
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWith = isHovered ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .padding = { 12, 12, 4, 4 } },
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
		.border = { .width=CLAY_BORDER_OUTSIDE(borderWith), .color=ToClayColor(borderColor) },
	});
	CLAY_TEXT(
		ToClayString(StrLit(btnText)),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayUiFontId,
			.fontSize = UI_FONT_SIZE,
			.textColor = ToClayColor(TEXT_WHITE),
		})
	);
	if (IsMouseOverClay(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left)) { *isOpenPntr = !*isOpenPntr; }
	if (*isOpenPntr == true && !isHovered && !keepOpen) { *isOpenPntr = false; }
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

bool ClayTopSubmenu(const char* btnText, bool isParentOpen, bool* isOpenPntr, r32 dropDownWidth, Texture* icon)
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
	u16 borderWith = isHovered ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .sizing = { .width=CLAY_SIZING_GROW(0), }, .padding = { 4, 4, 16, 16 } },
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
		.border = { .width=CLAY_BORDER_OUTSIDE(borderWith), .color=ToClayColor(borderColor) },
	});
	CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = TOPBAR_ICONS_PADDING, .padding = { .right = 8 }, } })
	{
		if (icon != nullptr)
		{
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE), TEXT_WHITE);
		}
		CLAY_TEXT(
			ToClayString(StrLit(btnText)),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = UI_FONT_SIZE,
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
				.sizing = {
					.width = CLAY_SIZING_FIXED(dropDownWidth),
				},
				.padding = CLAY_PADDING_ALL(2),
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
bool ClayBtnStrEx(Str8 idStr, Str8 btnText, bool isEnabled, Texture* icon)
{
	ScratchBegin(scratch);
	Str8 fullIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(idStr));
	ClayId btnId = ToClayId(fullIdStr);
	bool isHovered = IsMouseOverClay(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Color32 backgroundColor = !isEnabled ? BACKGROUND_BLACK : (isPressed ? SELECTED_BLUE : (isHovered ? HOVERED_BLUE : Transparent));
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWith = (isHovered && isEnabled) ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.padding = { .top = 8, .bottom = 8, .left = 4, .right = 4, },
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
		.border = { .width=CLAY_BORDER_OUTSIDE(borderWith), .color=ToClayColor(borderColor) },
	});
	CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = TOPBAR_ICONS_PADDING, .padding = { .right = 8 }, } })
	{
		if (icon != nullptr)
		{
			CLAY_ICON(icon, FillV2(TOPBAR_ICONS_SIZE), TEXT_WHITE);
		}
		CLAY_TEXT(
			ToClayString(btnText),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayUiFontId,
				.fontSize = MAIN_FONT_SIZE,
				.textColor = ToClayColor(TEXT_WHITE),
			})
		);
	}
	ScratchEnd(scratch);
	return (isHovered && isEnabled && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}
bool ClayBtnStr(Str8 btnText, bool isEnabled, Texture* icon)
{
	return ClayBtnStrEx(btnText, btnText, isEnabled, icon);
}
bool ClayBtn(const char* btnText, bool isEnabled, Texture* icon)
{
	return ClayBtnStr(StrLit(btnText), isEnabled, icon);
}

//Call Clay__CloseElement once after if statement
bool ClayOptionBtn(Str8 nameStr, Str8 valueStr, bool enabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(nameStr));
	ClayId btnId = ToClayId(btnIdStr);
	bool isHovered = IsMouseOverClay(btnId);
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
			.padding = CLAY_PADDING_ALL(4),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(isPressed ? pressColor : (isHovered ? hoverColor : backColor)),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
		.border = {
			.color = ToClayColor(outlineColor),
			.width = CLAY_BORDER_OUTSIDE(2),
		},
	});
	CLAY_TEXT(
		ToClayString(nameStr),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayMainFontId,
			.fontSize = MAIN_FONT_SIZE,
			.textColor = ToClayColor(textColor),
			.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
			.userData = { .contraction = app->clipNamesOnLeftSide ? TextContraction_EllipseLeft : TextContraction_EllipseRight },
		})
	);
	CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
	if (!IsEmptyStr(valueStr))
	{
		CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(4) } } }) {} //ensure 4px padding between name and value
		CLAY_TEXT(
			ToClayString(valueStr),
			CLAY_TEXT_CONFIG({
				.fontId = app->clayMainFontId,
				.fontSize = MAIN_FONT_SIZE,
				.textColor = ToClayColor(valueColor),
			})
		);
	}
	ScratchEnd(scratch);
	return (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}