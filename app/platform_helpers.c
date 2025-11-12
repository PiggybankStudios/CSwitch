/*
File:   platform_helpers.c
Author: Taylor Robbins
Date:   03\02\2025
Description: 
	** Holds various functions that the platform layer can use
*/

void InitAppInput(AppInput* input)
{
	NotNull(input);
	ClearPointer(input);
	InitKeyboardState(&input->keyboard);
	InitMouseState(&input->mouse);
	input->cursorType = SAPP_MOUSECURSOR_DEFAULT;
	input->isFullscreen = sapp_is_fullscreen();
	input->isFocused = true; //we assume we are focused when we first present the window, I belive sokol_app will give us an unfocus event immediately after startup if that's not true
	input->screenSize = MakeV2i((i32)sapp_width(), (i32)sapp_height());
	InitVarArray(Str8, &input->droppedFilePaths, stdHeap);
}