/*
File:   app_tooltips.c
Author: Taylor Robbins
Date:   03\07\2025
Description: 
	** Holds functions that manage a VarArray of TooltipRegions and the single
	** TooltipState struct that displays whenever one of those regions is hovered
	** over for some period of time (and disappears automatically when the mouse
	** moves or other app changes)
*/

void InitTooltipState(Arena* arena, TooltipState* tooltip)
{
	NotNull(arena);
	NotNull(tooltip);
	ClearPointer(tooltip);
	tooltip->arena = arena;
}

void CloseTooltip(TooltipState* tooltip)
{
	NotNull(tooltip);
	if (tooltip->isOpen)
	{
		FreeStr8(tooltip->arena, &tooltip->displayStr);
		tooltip->isOpen = false;
	}
}

TooltipRegion* FindTooltipRegionById(VarArray* tooltipRegions, u64 id)
{
	VarArrayLoop(tooltipRegions, rIndex)
	{
		VarArrayLoopGet(TooltipRegion, region, tooltipRegions, rIndex);
		if (region->id == id) { return region; }
	}
	return nullptr;
}

void UpdateTooltipPlacement(TooltipState* tooltip, v2 screenSize)
{
	if (tooltip->isOpen)
	{
		tooltip->mainRec.Size = Add(tooltip->displayStrMeasure.logicalRec.Size, Mul(TOOLTIP_TEXT_MARGIN, 2.0f));
		if (tooltip->mainRec.Width >= (r32)screenSize.Width) { tooltip->mainRec.Width = (r32)screenSize.Width; }
		tooltip->mainRec.X = tooltip->focusPos.X - tooltip->mainRec.Width/2;
		if (tooltip->mainRec.X < 0) { tooltip->mainRec.X = 0; }
		if (tooltip->mainRec.X > screenSize.Width - tooltip->mainRec.Width) { tooltip->mainRec.X = screenSize.Width - tooltip->mainRec.Width; }
		tooltip->mainRec.Y = tooltip->focusPos.Y - TOOLTIP_MOUSE_UP_OFFSET - tooltip->mainRec.Height;
		if (tooltip->mainRec.Y < 0)
		{
			tooltip->mainRec.Y = tooltip->focusPos.Y + TOOLTIP_MOUSE_DOWN_OFFSET;
			if (tooltip->mainRec.Y + tooltip->mainRec.Height >= screenSize.Height)
			{
				tooltip->mainRec.Y = 0;
			}
		}
		AlignV2(&tooltip->mainRec.TopLeft);
	}
}

void ShowTooltip(TooltipState* tooltip, TooltipRegion* region)
{
	NotNull(tooltip);
	NotNull(region);
	if (tooltip->isOpen) { CloseTooltip(tooltip); }
	tooltip->regionId = region->id;
	tooltip->displayStr = AllocStr8(tooltip->arena, region->displayStr);
	tooltip->displayStrMeasure = MeasureTextEx(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE, tooltip->displayStr);
	tooltip->focusPos = appIn->mouse.position;
	tooltip->isOpen = true;
	UpdateTooltipPlacement(tooltip, ToV2Fromi(appIn->screenSize));
}

void UpdateTooltipState(VarArray* tooltipRegions, TooltipState* tooltip)
{
	NotNull(tooltipRegions);
	NotNull(tooltip);
	NotNull(tooltip->arena);
	NotNull(appIn);
	v2 mousePos = appIn->mouse.position;
	u64 programTime = appIn->programTime;
	
	if (tooltip->isOpen)
	{
		bool shouldClose = false;
		
		if (appIn->isMinimized) { shouldClose = true; }
		if (!appIn->mouse.isOverWindow) { shouldClose = true; }
		if (Length(Sub(mousePos, tooltip->focusPos)) >= TOOLTIP_MAX_MOVE_DIST) { shouldClose = true; }
		TooltipRegion* region = FindTooltipRegionById(tooltipRegions, tooltip->regionId);
		if (region == nullptr) { shouldClose = true; }
		else if (!IsInsideRec(region->mainRec, mousePos)) { shouldClose = true; }
		
		if (shouldClose) { CloseTooltip(tooltip); }
	}
	else
	{
		TooltipRegion* hoveredRegion = nullptr;
		VarArrayLoop(tooltipRegions, rIndex)
		{
			VarArrayLoopGet(TooltipRegion, region, tooltipRegions, rIndex);
			if (IsInsideRec(region->mainRec, mousePos)) { hoveredRegion = region; break; }
		}
		
		bool resetMoveTracking = false;
		
		if (tooltip->lastMouseMoveTime == 0) { resetMoveTracking = true; }
		else if (hoveredRegion == nullptr && tooltip->lastMouseMoveRegionId != 0) { resetMoveTracking = true; }
		else if (hoveredRegion != nullptr && tooltip->lastMouseMoveRegionId != hoveredRegion->id) { resetMoveTracking = true; }
		else if (Length(Sub(mousePos, tooltip->lastMouseMovePos)) >= TOOLTIP_MAX_MOVE_DIST) { resetMoveTracking = true; }
		else if (hoveredRegion != nullptr && TimeSinceBy(programTime, tooltip->lastMouseMoveTime) >= hoveredRegion->delay)
		{
			ShowTooltip(tooltip, hoveredRegion);
		}
		
		if (resetMoveTracking)
		{
			tooltip->lastMouseMoveTime = programTime;
			tooltip->lastMouseMovePos = mousePos;
			tooltip->lastMouseMoveRegionId = (hoveredRegion != nullptr) ? hoveredRegion->id : 0;
		}
	}
}

void RenderTooltip(TooltipState* tooltip)
{
	NotNull(tooltip);
	if (tooltip->isOpen)
	{
		UpdateTooltipPlacement(tooltip, ToV2Fromi(appIn->screenSize));
		DrawRectangle(tooltip->mainRec, TEXT_WHITE);
		FontAtlas* fontAtlas = GetFontAtlas(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE);
		NotNull(fontAtlas);
		v2 textPos = NewV2(
			tooltip->mainRec.X + tooltip->mainRec.Width/2 - tooltip->displayStrMeasure.Width/2,
			tooltip->mainRec.Y + tooltip->mainRec.Height/2 + fontAtlas->centerOffset
		);
		AlignV2(&textPos);
		reci oldClipRec = AddClipRec(ToReciFromf(tooltip->mainRec));
		BindFontEx(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE);
		DrawText(tooltip->displayStr, textPos, BACKGROUND_DARK_GRAY);
		SetClipRec(oldClipRec);
		DrawRectangleOutline(tooltip->mainRec, 1, TEXT_LIGHT_GRAY);
	}
}
