/*
File:   app_popup_dialog.c
Author: Taylor Robbins
Date:   03\14\2025
Description: 
	** Holds functions that help us create, manage, and close the PopupDialog structure
	** that displays a message to the user, presenting them with a choice between
	** some number of buttons. Often this is just "Ok" or "Yes"\"No" but the buttons
	** can say whatever the invoking code wants, and be interepreted by the callback
	** once the user makes a choice. Most dialogs have an option to press Escape
	** to dismiss them, which usually will be treated by the callback to be synonymous
	** with one of the options presented.
*/

void FreePopupDialogButton(PopupDialogButton* button)
{
	NotNull(button);
	if (button->arena != nullptr)
	{
		FreeStr8(button->arena, &button->displayStr);
	}
	ClearPointer(button);
}

PopupDialogButton* AddPopupButton(PopupDialog* dialog, u64 buttonId, Str8 displayStr, PopupDialogResult buttonResult, Color32 buttonColor)
{
	NotNull(dialog);
	NotNull(dialog->arena);
	Assert(buttonId != 0);
	PopupDialogButton* newButton = VarArrayAdd(PopupDialogButton, &dialog->buttons);
	NotNull(newButton);
	ClearPointer(newButton);
	newButton->arena = dialog->arena;
	newButton->id = buttonId;
	newButton->result = buttonResult;
	newButton->color = buttonColor;
	newButton->displayStr = AllocStr8(dialog->arena, displayStr);
	return newButton;
}

void FreePopupDialog(PopupDialog* dialog)
{
	NotNull(dialog);
	if (dialog->arena != nullptr)
	{
		FreeStr8(dialog->arena, &dialog->messageStr);
		FreeVarArray(&dialog->buttons);
	}
	ClearPointer(dialog);
}

void ClosePopupDialog(PopupDialog* dialog, PopupDialogButton* chosenButton)
{
	NotNull(appIn);
	if (dialog->isOpen)
	{
		dialog->isOpen = false;
		dialog->closeTime = appIn->programTime;
		
		if (dialog->callback != nullptr)
		{
			//TODO: Should we make a copy of the PopupDialog struct or PopupDialogButton? If the callback spawns a new dialog, the pointers will become invalid
			((PopupDialogCallback_f*)dialog->callback)(chosenButton != nullptr ? chosenButton->result : PopupDialogResult_Closed, dialog, chosenButton, dialog->callbackContext);
		}
	}
}

void OpenPopupDialog(Arena* arena, PopupDialog* dialog, Str8 messageStr, PopupDialogCallback_f* callback, void* callbackContext)
{
	NotNull(appIn);
	NotNull(arena);
	NotNull(dialog);
	NotNullStr(messageStr);
	if (dialog->arena != nullptr) { FreePopupDialog(dialog); }
	dialog->arena = arena;
	dialog->messageStr = AllocStr8(arena, messageStr);
	InitVarArray(PopupDialogButton, &dialog->buttons, arena);
	dialog->isOpen = true;
	dialog->isVisible = true;
	dialog->openTime = appIn->programTime;
	dialog->result = PopupDialogResult_None;
	dialog->callback = (void*)callback;
	dialog->callbackContext = callbackContext;
}

void UpdatePopupDialog(PopupDialog* dialog)
{
	NotNull(appIn);
	NotNull(dialog);
	if (!dialog->isVisible) { return; }
	NotNull(dialog->arena);
	
	if (!dialog->isOpen)
	{
		if (TimeSinceBy(appIn->programTime, dialog->closeTime) >= POPUP_CLOSE_ANIM_TIME)
		{
			FreePopupDialog(dialog);
		}
		return;
	}
	
	VarArrayLoop(&dialog->buttons, bIndex)
	{
		VarArrayLoopGet(PopupDialogButton, button, &dialog->buttons, bIndex);
		Str8 buttonClayIdStr = ScratchPrintStr("PopupDialogBtn[%llu]_%s", button->id, GetPopupDialogResultStr(button->result));
		ClayId buttonClayId = ToClayId(buttonClayIdStr);
		bool isButtonHovered = IsMouseOverClay(buttonClayId);
		if (isButtonHovered && MouseLeftClicked())
		{
			dialog->result = button->result;
			ClosePopupDialog(dialog, button);
			return;
		}
	}
}

void RenderPopupDialog(PopupDialog* dialog)
{
	NotNull(appIn);
	NotNull(dialog);
	if (!dialog->isVisible) { return; }
	
	r32 openAnimAmount = 1.0f;
	if (dialog->isOpen)
	{
		u64 openAnimTime = TimeSinceBy(appIn->programTime, dialog->openTime);
		openAnimAmount = ClampR32((r32)openAnimTime / (r32)POPUP_OPEN_ANIM_TIME, 0.0f, 1.0f);
	}
	r32 closeAnimAmount = 0.0f;
	if (!dialog->isOpen)
	{
		u64 closeAnimTime = TimeSinceBy(appIn->programTime, dialog->closeTime);
		closeAnimAmount = ClampR32((r32)closeAnimTime / (r32)POPUP_CLOSE_ANIM_TIME, 0.0f, 1.0f);
	}
	r32 dialogAlpha = openAnimAmount * (1.0f - closeAnimAmount);
	r32 dialogMaxWidth = (r32)appIn->screenSize.Width * EaseQuadraticOut(dialogAlpha);
	r32 textAlpha = InverseLerpR32(0.75f, 1.0f, dialogAlpha);
	Color32 darkenColor = GetThemeColor(ConfirmDialogDarken);
	darkenColor = ColorWithAlpha(darkenColor, (darkenColor.alpha/255.0f) * dialogAlpha);
	Color32 dialogColor = ColorWithAlpha(GetThemeColor(ConfirmDialogBack), dialogAlpha);
	Color32 textColor = ColorWithAlpha(GetThemeColor(ConfirmDialogText), textAlpha);
	
	CLAY({ .id = CLAY_ID("PopupDialogScreenOverlay"),
		.layout = {
			.childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
			.sizing = { .width = CLAY_SIZING_FIXED((r32)appIn->screenSize.Width), .height = CLAY_SIZING_FIXED((r32)appIn->screenSize.Height) },
		},
		.floating = {
			.zIndex = 100,
			.attachTo = CLAY_ATTACH_TO_PARENT,
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
			.attachPoints = {
				.parent = CLAY_ATTACH_POINT_LEFT_TOP,
				.element = CLAY_ATTACH_POINT_LEFT_TOP,
			},
		},
		.backgroundColor = darkenColor,
	})
	{
		CLAY({ .id = CLAY_ID("PopupDialog"),
			.layout = {
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.padding = CLAY_PADDING_ALL(UI_U16(16)),
				.sizing = {
					.width = CLAY_SIZING_FIT(MinR32(POPUP_MIN_WIDTH * app->uiScale, dialogMaxWidth), dialogMaxWidth),
					.height = CLAY_SIZING_FIT(MinR32(POPUP_MIN_HEIGHT * app->uiScale, (r32)appIn->screenSize.Height), (r32)appIn->screenSize.Height)
				},
			},
			.backgroundColor = dialogColor,
			.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(8)),
			.border = { .width = CLAY_BORDER_OUTSIDE(UI_BORDER(2)), .color = GetThemeColor(ConfirmDialogBorder) },
		})
		{
			CLAY({ .layout={ .sizing={ .height=CLAY_SIZING_GROW(UI_U16(10), 0) } } }) {}
			
			CLAY_TEXT(
				dialog->messageStr,
				CLAY_TEXT_CONFIG({
					.fontId = app->clayUiFontId,
					.fontSize = (u16)app->uiFontSize,
					.textColor = textColor,
				})
			);
			
			CLAY({ .layout={ .sizing={ .height=CLAY_SIZING_GROW(UI_U16(20), 0) } } }) {}
			
			CLAY({ .id=CLAY_ID("PopupDialogButtonsList"),
				.layout = {
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
					.childGap = UI_U16(8),
					.sizing = { .width=CLAY_SIZING_PERCENT(1.0f) },
				},
			})
			{
				CLAY({ .layout={ .sizing={ .width=CLAY_SIZING_GROW(0) } } }) {}
				
				VarArrayLoop(&dialog->buttons, bIndex)
				{
					VarArrayLoopGet(PopupDialogButton, button, &dialog->buttons, bIndex);
					
					Str8 buttonClayIdStr = ScratchPrintStr("PopupDialogBtn[%llu]_%s", button->id, GetPopupDialogResultStr(button->result));
					ClayId buttonClayId = ToClayId(buttonClayIdStr);
					bool isButtonHovered = IsMouseOverClay(buttonClayId);
					Color32 buttonColor = Transparent;
					Color32 borderColor = ColorWithAlpha(button->color, 0.5f * dialogAlpha);
					if (isButtonHovered && dialog->isOpen)
					{
						buttonColor = ColorWithAlpha(button->color, 0.5f * dialogAlpha);
						borderColor = ColorWithAlpha(button->color, dialogAlpha);
					}
					
					r32 maxButtonWidth = (dialogMaxWidth - (UI_U16(16)*2.0f)) / (r32)dialog->buttons.length;
					CLAY({ .id = buttonClayId,
						.layout = {
							.padding = CLAY_PADDING_ALL(UI_U16(8)),
							.sizing = {
								.width = CLAY_SIZING_FIT(MinR32(100, maxButtonWidth), (r32)appIn->screenSize.Width/(r32)dialog->buttons.length),
								.height = CLAY_SIZING_FIT(0, (r32)appIn->screenSize.Height) },
							.childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
						},
						.backgroundColor = buttonColor,
						.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(9)),
						.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(2)), .color=borderColor, }
					})
					{
						CLAY_TEXT(
							button->displayStr,
							CLAY_TEXT_CONFIG({
								.fontId = app->clayUiFontId,
								.fontSize = (u16)app->uiFontSize,
								.textColor = ColorWithAlpha(GetThemeColor(ConfirmDialogText), textAlpha), //TODO: $THEME Update to use color for the type of button (defined by the theme)
							})
						);
					}
				}
			}
		}
	}
}
