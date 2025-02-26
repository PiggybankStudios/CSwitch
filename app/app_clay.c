/*
File:   app_clay.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** Holds various functions that comprise common widgets for Clay
*/

//Call Clay__CloseElement once if false, three times if true (i.e. twicfe inside the if statement and once after)
bool ClayTopBtn(const char* btnText, bool* isOpenPntr, Color32 backColor, Color32 textColor, r32 dropDownWidth)
{
	ScratchBegin(scratch);
	Color32 highlightColor = ColorLerpSimple(backColor, White, 0.3f);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%s_TopBtn", btnText);
	Str8 menuIdStr = PrintInArenaStr(scratch, "%s_TopBtnMenu", btnText);
	Clay_ElementId btnId = ToClayId(btnIdStr);
	Clay_ElementId menuId = ToClayId(menuIdStr);
	bool isBtnHoveredOrMenuOpen = (Clay_PointerOver(btnId) || *isOpenPntr);
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = { .padding = { 6, 6, 4, 4 } },
		.backgroundColor = ToClayColor(isBtnHoveredOrMenuOpen ? highlightColor : backColor),
		.cornerRadius = CLAY_CORNER_RADIUS(5),
	});
	CLAY_TEXT(
		ToClayString(StrLit(btnText)),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayUiFontId,
			.fontSize = UI_FONT_SIZE,
			.textColor = ToClayColor(textColor),
		})
	);
	bool isHovered = (Clay_PointerOver(btnId) || Clay_PointerOver(menuId));
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
				.padding = { 0, 0, 0, 0 },
			}
		});
		
		Clay__OpenElement();
		Clay__ConfigureOpenElement((Clay_ElementDeclaration){
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.sizing = {
					.width = CLAY_SIZING_FIXED(dropDownWidth),
				},
				.childGap = 2,
			},
			.backgroundColor = ToClayColor(MonokaiBack),
			.cornerRadius = CLAY_CORNER_RADIUS(8),
		});
	}
	ScratchEnd(scratch);
	return *isOpenPntr;
}

//Call Clay__CloseElement once after if statement
bool ClayBtnStr(Str8 btnText, Color32 backColor, Color32 textColor, bool isEnabled)
{
	ScratchBegin(scratch);
	Color32 hoverColor = ColorLerpSimple(backColor, White, 0.3f);
	Color32 pressColor = ColorLerpSimple(backColor, White, 0.1f);
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(btnText));
	Clay_ElementId btnId = ToClayId(btnIdStr);
	bool isHovered = Clay_PointerOver(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.padding = CLAY_PADDING_ALL(4),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor((isPressed || !isEnabled) ? pressColor : (isHovered ? hoverColor : backColor)),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
	});
	CLAY_TEXT(
		ToClayString(btnText),
		CLAY_TEXT_CONFIG({
			.fontId = app->clayUiFontId,
			.fontSize = MAIN_FONT_SIZE,
			.textColor = ToClayColor(textColor),
		})
	);
	ScratchEnd(scratch);
	return (isHovered && isEnabled && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}
bool ClayBtn(const char* btnText, Color32 backColor, Color32 textColor, bool isEnabled)
{
	return ClayBtnStr(StrLit(btnText), backColor, textColor, isEnabled);
}

//Call Clay__CloseElement once after if statement
bool ClayOptionBtn(Str8 nameStr, Str8 valueStr, bool enabled)
{
	ScratchBegin(scratch);
	Color32 hoverColor = ColorLerpSimple(MonokaiBack, White, 0.3f);
	Color32 pressColor = ColorLerpSimple(MonokaiBack, White, 0.1f);
	Color32 textColor = enabled ? MonokaiGreen : MonokaiGray1;
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_Btn", StrPrint(nameStr));
	Clay_ElementId btnId = ToClayId(btnIdStr);
	bool isHovered = Clay_PointerOver(btnId);
	bool isPressed = (isHovered && IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
	Clay__OpenElement();
	Clay__ConfigureOpenElement((Clay_ElementDeclaration){
		.id = btnId,
		.layout = {
			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.padding = CLAY_PADDING_ALL(4),
			.sizing = { .width = CLAY_SIZING_GROW(0), },
		},
		.backgroundColor = ToClayColor(isPressed ? pressColor : (isHovered ? hoverColor : MonokaiBack)),
		.cornerRadius = CLAY_CORNER_RADIUS(4),
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
			.textColor = ToClayColor(textColor),
		})
	);
	ScratchEnd(scratch);
	return (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left));
}