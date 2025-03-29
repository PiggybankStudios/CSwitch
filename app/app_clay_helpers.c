/*
File:   app_clay_helpers.c
Author: Taylor Robbins
Date:   03\11\2025
Description: 
	** A small set of Clay related helper functions and macros that are included earlier
	** than app_clay.c because the are relied on by files that are included before app_clay.c
*/

// These macros are really shorthand for multiplying by app->uiScale and dealing with how to round or clamp to good values when uiScale is very big/small
#define UI_R32(pixels) RoundR32((r32)(pixels) * app->uiScale)
#define UI_U16(pixels) (u16)RoundR32i((r32)(pixels) * app->uiScale)
#define UI_BORDER(pixels) MaxU16(((pixels) > 0) ? (u16)1 : (u16)0, (u16)RoundR32i((r32)(pixels) * app->uiScale))

bool IsMouseOverClay(ClayId clayId)
{
	return appIn->mouse.isOverWindow && Clay_PointerOver(clayId);
}
bool IsMouseOverClayInContainer(ClayId containerId, ClayId clayId)
{
	return appIn->mouse.isOverWindow && Clay_PointerOver(containerId) && Clay_PointerOver(clayId);
}

// This should produce the same value as ClayUIRendererMeasureText in gfx_clay_renderer.h
v2 ClayUiTextSize(PigFont* font, r32 fontSize, u8 styleFlags, Str8 text)
{
	TextMeasure textMeasure = MeasureTextEx(font, fontSize, styleFlags, text);
	return NewV2(CeilR32(textMeasure.Width - textMeasure.OffsetX), CeilR32(textMeasure.Height));
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
			.Width = (r32)((texturePntr)->Width),       \
			.Height = (r32)((texturePntr)->Height),     \
		},                                              \
	},                                                  \
	.backgroundColor = color,                           \
}) {}
