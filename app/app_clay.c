/*
File:   app_clay.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** Holds various functions that comprise common widgets for Clay
*/

//Call Clay__CloseElement once if false, three times if true (i.e. twicfe inside the if statement and once after)
bool ClayTopBtn(const char* btnText, bool* isOpenPntr, r32 dropDownWidth)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopBtn", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopBtnMenu", btnText);
	Clay_ElementId btnId = ToClayId(btnIdStr);
	Clay_ElementId menuId = ToClayId(menuIdStr);
	bool isBtnHovered = Clay_PointerOver(btnId);
	bool isHovered = (isBtnHovered || Clay_PointerOver(menuId));
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
	if (Clay_PointerOver(btnId) && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left)) { *isOpenPntr = !*isOpenPntr; }
	if (*isOpenPntr == true && !isHovered) { *isOpenPntr = false; }
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
bool ClayBtnStr(Str8 btnText, bool isEnabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(btnText));
	Clay_ElementId btnId = ToClayId(btnIdStr);
	bool isHovered = Clay_PointerOver(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Color32 backgroundColor = !isEnabled ? BACKGROUND_BLACK : (isPressed ? SELECTED_BLUE : (isHovered ? HOVERED_BLUE : Transparent));
	Color32 borderColor = SELECTED_BLUE;
	u16 borderWith = (isHovered && isEnabled) ? 1 : 0;
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.padding = CLAY_PADDING_ALL(4),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(backgroundColor),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
		.border = { .width=CLAY_BORDER_OUTSIDE(borderWith), .color=ToClayColor(borderColor) },
	});
	CLAY_TEXT(
		ToClayString(btnText),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayUiFontId,
			.fontSize = MAIN_FONT_SIZE,
			.textColor = ToClayColor(TEXT_WHITE),
		})
	);
	ScratchEnd(scratch);
	return (isHovered && isEnabled && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}
bool ClayBtn(const char* btnText, bool isEnabled)
{
	return ClayBtnStr(StrLit(btnText), isEnabled);
}

//Call Clay__CloseElement once after if statement
bool ClayOptionBtn(Str8 nameStr, Str8 valueStr, bool enabled)
{
	ScratchBegin(scratch);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(nameStr));
	Clay_ElementId btnId = ToClayId(btnIdStr);
	bool isHovered = Clay_PointerOver(btnId);
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
		})
	);
	CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } });
	CLAY_TEXT(
		ToClayString(valueStr),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayMainFontId,
			.fontSize = MAIN_FONT_SIZE,
			.textColor = ToClayColor(valueColor),
		})
	);
	ScratchEnd(scratch);
	return (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}