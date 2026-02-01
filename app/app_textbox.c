/*
File:   app_textbox.c
Author: Taylor Robbins
Date:   03\23\2025
Description: 
	** Holds functions that handle an box with text that the user can type into (also selection using the mouse)
*/

#if 0
void FreeClayTextbox(ClayTextbox* textbox)
{
	NotNull(textbox);
	if (textbox->edit.arena != nullptr)
	{
		FreeVarArray(&textbox->edit.strBuffer);
		FreeStr8(textbox->edit.arena, &textbox->idStr);
		FreeStr8(textbox->edit.arena, &textbox->hintStr);
		FreeVarArray(&textbox->flowGlyphs);
	}
	ClearPointer(textbox);
}

void InitClayTextbox(Arena* arena, Str8 idStr, Str8 initialText, u16 clayFontId, r32 fontSize, ClayTextbox* textboxOut)
{
	NotNull(arena);
	NotNullStr(idStr);
	NotEmptyStr(idStr);
	NotNullStr(initialText);
	Assert(clayFontId != 0);
	NotNull(textboxOut);
	
	ClearPointer(textboxOut);
	textboxOut->edit.arena = arena;
	
	InitVarArrayWithInitial(char, &textboxOut->edit.strBuffer, arena, initialText.length);
	textboxOut->edit.str = MakeStr8(initialText.length, (char*)textboxOut->edit.strBuffer.items);
	if (initialText.length > 0)
	{
		MyMemCopy(textboxOut->edit.str.chars, initialText.chars, initialText.length);
	}
	textboxOut->edit.cursorActive = true;
	textboxOut->edit.cursorEnd = textboxOut->edit.str.length;
	textboxOut->edit.cursorStart = textboxOut->edit.cursorEnd;
	
	textboxOut->idStr = AllocStr8(arena, idStr);
	textboxOut->clayFontId = clayFontId;
	textboxOut->fontSize = fontSize;
	InitVarArrayWithInitial(FontFlowGlyph, &textboxOut->flowGlyphs, arena, initialText.length);
}

void EditTextResetCursorBlink(EditableText* editText)
{
	NotNull(appIn);
	editText->cursorMoveTime = appIn->programTime;
}

void EditTextDeleteRange(EditableText* editText, uxx rangeStart, uxx rangeEnd)
{
	NotNull(editText);
	NotNull(editText->arena);
	Assert(rangeStart <= editText->str.length);
	Assert(rangeEnd <= editText->str.length);
	if (rangeStart == rangeEnd) { return; }
	uxx rangeMin = MinUXX(rangeStart, rangeEnd);
	uxx rangeMax = MaxUXX(rangeStart, rangeEnd);
	if (rangeMax < editText->str.length)
	{
		MyMemMove(&editText->str.chars[rangeMin], &editText->str.chars[rangeMax], editText->str.length - rangeMax);
	}
	editText->str.length = rangeMin + (editText->str.length - rangeMax);
	editText->strBuffer.length = editText->str.length;
	if (editText->cursorActive)
	{
		if (editText->cursorStart >= rangeMax) { editText->cursorStart -= (rangeMax - rangeMin); }
		else if (editText->cursorStart > rangeMin) { editText->cursorStart = rangeMin; }
		if (editText->cursorEnd >= rangeMax) { editText->cursorEnd -= (rangeMax - rangeMin); }
		else if (editText->cursorEnd > rangeMin) { editText->cursorEnd = rangeMin; }
		EditTextResetCursorBlink(editText);
	}
	editText->textChanged = true;
}

void EditTextInsertAt(EditableText* editText, Str8 newText, uxx insertionIndex)
{
	NotNull(editText);
	NotNull(editText->arena);
	Assert(insertionIndex <= editText->str.length);
	if (!editText->cursorActive) { return; }
	if (newText.length == 0) { return; }
	VarArrayExpand(&editText->strBuffer, editText->str.length + newText.length);
	editText->str.chars = (char*)editText->strBuffer.items;
	if (editText->str.length > insertionIndex)
	{
		MyMemMove(&editText->str.chars[insertionIndex + newText.length], &editText->str.chars[insertionIndex], editText->str.length - insertionIndex);
	}
	MyMemCopy(&editText->str.chars[insertionIndex], newText.chars, newText.length);
	editText->str.length += newText.length;
	editText->strBuffer.length = editText->str.length;
	if (editText->cursorActive)
	{
		if (editText->cursorStart >= insertionIndex) { editText->cursorStart += newText.length; }
		if (editText->cursorEnd >= insertionIndex) { editText->cursorEnd += newText.length; }
		EditTextResetCursorBlink(editText);
	}
	editText->textChanged = true;
}

bool EditTextHandleCharInput(EditableText* editText, KeyboardCharInput* charInput)
{
	NotNull(editText);
	NotNull(editText->arena);
	if (!editText->isFocused) { return false; }
	if (!editText->cursorActive) { return false; }
	if (IsFlagSet(charInput->modifierKeys, ModifierKey_Control) || IsFlagSet(charInput->modifierKeys, ModifierKey_Alt)) { return false; }
	u8 utf8Bytes[UTF8_MAX_CHAR_SIZE];
	u8 utf8ByteSize = GetUtf8BytesForCode(charInput->codepoint, &utf8Bytes[0], false);
	if (utf8ByteSize == 0) { return false; } //codepoint can't be UTF-8 encoded!
	if (editText->cursorStart != editText->cursorEnd) { EditTextDeleteRange(editText, editText->cursorStart, editText->cursorEnd); }
	EditTextInsertAt(editText, MakeStr8(utf8ByteSize, (char*)&utf8Bytes[0]), editText->cursorEnd);
	EditTextResetCursorBlink(editText);
	return true;
}

void UpdateClayTextbox(ClayTextbox* textbox)
{
	NotNull(textbox);
	Assert(textbox->clayFontId != 0);
	
	ScratchBegin1(scratch, textbox->edit.arena);
	ClayUIRendererFont* clayFont = VarArrayGetHard(ClayUIRendererFont, &app->clay.fonts, (uxx)textbox->clayFontId);
	NotNull(clayFont);
	PigFont* font = clayFont->pntr;
	ClayId textboxId = ToClayId(textbox->idStr);
	rec textboxDrawRec = GetClayElementDrawRec(textboxId);
	bool isBoxHovered = IsMouseOverClay(textboxId);
	
	// +==============================+
	// |   Handle mainRec Movements   |
	// +==============================+
	textbox->mainRecChanged = false;
	if (textboxDrawRec.Width > 0 && textboxDrawRec.Height > 0)
	{
		if (!AreEqualRec(textbox->mainRec, textboxDrawRec))
		{
			textbox->mainRecChanged = true;
			textbox->mainRec = textboxDrawRec;
			AlignRec(&textbox->mainRec);
			textbox->textPos = MakeV2(
				textboxDrawRec.X + UI_U16(4),
				textboxDrawRec.Y + textboxDrawRec.Height/2 + GetFontCenterOffset(font, textbox->fontSize, clayFont->styleFlags)
			);
			AlignV2(&textbox->textPos);
			//TODO: Handle any input fixups to account for textbox moving!
			//TODO: Scroll the view left/right to keep the cursor in focus!
		}
	}
	
	// +===============================+
	// | Measure Text on Changed/Moved |
	// +===============================+
	if (textbox->edit.textChanged || textbox->mainRecChanged)
	{
		textbox->edit.textChanged = false;
		
		FontFlow flow = ZEROED;
		VarArrayExpand(&textbox->flowGlyphs, textbox->edit.str.length);
		flow.numGlyphsAlloc = textbox->flowGlyphs.allocLength;
		flow.glyphs = (FontFlowGlyph*)textbox->flowGlyphs.items;
		
		textbox->measure = MeasureTextFlow(font, textbox->fontSize, clayFont->styleFlags, false, 0.0f, textbox->edit.str, &flow);
		
		Assert(flow.numGlyphs <= textbox->flowGlyphs.allocLength);
		textbox->flowGlyphs.length = flow.numGlyphs;
	}
	
	// +================================+
	// | Find Cursor Index When Hovered |
	// +================================+
	if (isBoxHovered && textbox->flowGlyphs.length > 0)
	{
		v2 relativeMousePos = Sub(appIn->mouse.position, textbox->textPos);
		
		// v2 closestPos = V2_Zero;
		r32 closestDistSquared = 0.0f;
		uxx closestIndex = 0;
		VarArrayLoop(&textbox->flowGlyphs, gIndex)
		{
			VarArrayLoopGet(FontFlowGlyph, glyph, &textbox->flowGlyphs, gIndex);
			v2 leftPos = glyph->position;
			r32 leftDistSquared = LengthSquaredV2(Sub(relativeMousePos, leftPos));
			if (gIndex == 0 || leftDistSquared < closestDistSquared)
			{
				// closestPos = leftPos;
				closestDistSquared = leftDistSquared;
				closestIndex = gIndex;
			}
			if (glyph->glyph != nullptr)
			{
				v2 rightPos = Add(glyph->position, MakeV2(glyph->glyph->metrics.advanceX, 0));
				r32 rightDistSquared = LengthSquaredV2(Sub(relativeMousePos, rightPos));
				if (rightDistSquared < closestDistSquared)
				{
					// closestPos = rightPos;
					closestDistSquared = rightDistSquared;
					closestIndex = gIndex + GetCodepointUtf8Size(glyph->codepoint);
				}
			}
		}
		
		Assert(closestIndex <= textbox->edit.str.length);
		textbox->edit.isMouseHovering = true;
		textbox->edit.mouseHoverIndex = closestIndex;
	}
	else { textbox->edit.isMouseHovering = false; }
	
	// +===============================+
	// | Handle MouseBtn_Left to Focus |
	// +===============================+
	if (!textbox->edit.isFocused && isBoxHovered && MouseLeftClicked())
	{
		textbox->edit.isFocused = true;
		EditTextResetCursorBlink(&textbox->edit);
	}
	if (textbox->edit.isFocused && !isBoxHovered && MouseLeftClicked())
	{
		textbox->edit.isFocused = false;
	}
	
	if (textbox->edit.isFocused)
	{
		// +==============================+
		// |      Handle Arrow Keys       |
		// +==============================+
		//TODO: Handle Ctrl and Alt modifiers!
		if (IsKeyboardKeyPressed(&appIn->keyboard, &appInputHandling->keyboard, Key_Left, true))
		{
			if (textbox->edit.cursorActive)
			{
				if (textbox->edit.cursorStart == textbox->edit.cursorEnd || IsKeyboardKeyDown(&appIn->keyboard, &appInputHandling->keyboard, Key_Shift))
				{
					if (textbox->edit.cursorEnd > 0)
					{
						textbox->edit.cursorEnd--;
						if (!IsKeyboardKeyDown(&appIn->keyboard, &appInputHandling->keyboard, Key_Shift)) { textbox->edit.cursorStart = textbox->edit.cursorEnd; }
					}
				}
				else
				{
					textbox->edit.cursorEnd = MinUXX(textbox->edit.cursorStart, textbox->edit.cursorEnd);
					textbox->edit.cursorStart = textbox->edit.cursorEnd;
				}
			}
			else
			{
				textbox->edit.cursorEnd = textbox->edit.str.length;
				textbox->edit.cursorStart = textbox->edit.cursorEnd;
			}
			EditTextResetCursorBlink(&textbox->edit);
		}
		if (IsKeyboardKeyPressed(&appIn->keyboard, &appInputHandling->keyboard, Key_Right, true))
		{
			if (textbox->edit.cursorActive)
			{
				if (textbox->edit.cursorStart == textbox->edit.cursorEnd || IsKeyboardKeyDown(&appIn->keyboard, &appInputHandling->keyboard, Key_Shift))
				{
					if (textbox->edit.cursorEnd < textbox->edit.str.length)
					{
						textbox->edit.cursorEnd++;
						if (!IsKeyboardKeyDown(&appIn->keyboard, &appInputHandling->keyboard, Key_Shift)) { textbox->edit.cursorStart = textbox->edit.cursorEnd; }
					}
				}
				else
				{
					textbox->edit.cursorEnd = MaxUXX(textbox->edit.cursorStart, textbox->edit.cursorEnd);
					textbox->edit.cursorStart = textbox->edit.cursorEnd;
				}
			}
			else
			{
				textbox->edit.cursorEnd = textbox->edit.str.length;
				textbox->edit.cursorStart = textbox->edit.cursorEnd;
			}
			EditTextResetCursorBlink(&textbox->edit);
		}
		
		// +==============================+
		// |       Handle Backspace       |
		// +==============================+
		if (IsKeyboardKeyPressed(&appIn->keyboard, &appInputHandling->keyboard, Key_Backspace, true))
		{
			if (!textbox->edit.cursorActive)
			{
				textbox->edit.cursorEnd = textbox->edit.str.length;
				textbox->edit.cursorStart = textbox->edit.cursorEnd;
				textbox->edit.cursorActive = true;
			}
			
			if (textbox->edit.cursorEnd != textbox->edit.cursorStart)
			{
				EditTextDeleteRange(&textbox->edit, textbox->edit.cursorStart, textbox->edit.cursorEnd);
			}
			else if (textbox->edit.cursorEnd > 0)
			{
				//TODO: This needs to actually figure out how big the previous encoded UTF-8 character is! It's not always 1 byte!
				EditTextDeleteRange(&textbox->edit, textbox->edit.cursorEnd-1, textbox->edit.cursorEnd);
			}
		}
		
		// +==============================+
		// |        Handle Delete         |
		// +==============================+
		if (IsKeyboardKeyPressed(&appIn->keyboard, &appInputHandling->keyboard, Key_Delete, true))
		{
			if (!textbox->edit.cursorActive)
			{
				textbox->edit.cursorEnd = textbox->edit.str.length;
				textbox->edit.cursorStart = textbox->edit.cursorEnd;
				textbox->edit.cursorActive = true;
			}
			
			if (textbox->edit.cursorEnd != textbox->edit.cursorStart)
			{
				EditTextDeleteRange(&textbox->edit, textbox->edit.cursorStart, textbox->edit.cursorEnd);
			}
			else if (textbox->edit.cursorEnd < textbox->edit.str.length)
			{
				//TODO: This needs to actually figure out how big the next encoded UTF-8 character is! It's not always 1 byte!
				EditTextDeleteRange(&textbox->edit, textbox->edit.cursorEnd, textbox->edit.cursorEnd+1);
			}
		}
		
		// +==============================+
		// |   Handle Typed Characters    |
		// +==============================+
		if (textbox->edit.isFocused && textbox->edit.cursorActive)
		{
			for (uxx cIndex = 0; cIndex < appIn->keyboard.numCharInputs; cIndex++)
			{
				KeyboardCharInput* charInput = &appIn->keyboard.charInputs[cIndex];
				if (!appInputHandling->keyboard.charInputHandled[cIndex])
				{
					bool inputHandled = EditTextHandleCharInput(&textbox->edit, charInput);
					if (inputHandled)
					{
						appInputHandling->keyboard.charInputHandled[cIndex] = true;
						//TODO: Handle the corresponding Key BtnState!
					}
				}
			}
		}
	}
	
	ScratchEnd(scratch);
}

void DrawClayTextbox(ClayTextbox* textbox, Clay_SizingAxis boxSizingWidth)
{
	NotNull(textbox);
	Assert(textbox->clayFontId != 0);
	
	ClayUIRendererFont* clayFont = VarArrayGetHard(ClayUIRendererFont, &app->clay.fonts, (uxx)textbox->clayFontId);
	NotNull(clayFont);
	u16 borderWidth = (textbox->edit.isFocused) ? 1 : 0;
	Color32 hintColor = (textbox->edit.str.length > 0 || textbox->hintStr.length == 0) ? Transparent : GetThemeColor(OptionOnNameText);
	Str8 hintStr = IsEmptyStr(textbox->hintStr) ? StrLit("W") : textbox->hintStr;
	r32 textboxHeight = GetFontLineHeight(clayFont->pntr, textbox->fontSize, clayFont->styleFlags) + UI_U16(4)*2.0f;
	
	CLAY({ .id = ToClayId(textbox->idStr),
		.layout = {
			.childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
			.padding = CLAY_PADDING_ALL(UI_U16(4)),
			.sizing = { .width=boxSizingWidth, .height=CLAY_SIZING_FIXED(textboxHeight) },
		},
		.border = { .width=CLAY_BORDER_OUTSIDE(UI_BORDER(borderWidth)), .color=GetThemeColor(OptionOnNameText) },
		.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(4)),
	})
	{
		CLAY_TEXT(
			hintStr,
			CLAY_TEXT_CONFIG({
				.fontId = textbox->clayFontId,
				.fontSize = (u16)textbox->fontSize,
				.textColor = hintColor,
				.wrapMode = CLAY_TEXT_WRAP_NONE,
				.textAlignment = CLAY_TEXT_ALIGN_CENTER,
			})
		);
	}
}

void DrawClayTextboxText(ClayTextbox* textbox)
{
	ScratchBegin1(scratch, textbox->edit.arena);
	ClayUIRendererFont* clayFont = VarArrayGetHard(ClayUIRendererFont, &app->clay.fonts, (uxx)textbox->clayFontId);
	NotNull(clayFont);
	rec alignedRec = textbox->mainRec;
	AlignRec(&alignedRec);
	reci oldClipRec = AddClipRec(ToReciFromf(alignedRec));
	
	RichStr highlightedText = ToRichStr(textbox->edit.str);
	if (textbox->edit.cursorActive && textbox->edit.cursorEnd != textbox->edit.cursorStart)
	{
		uxx selectionMin = MinUXX(textbox->edit.cursorStart, textbox->edit.cursorEnd);
		uxx selectionMax = MaxUXX(textbox->edit.cursorStart, textbox->edit.cursorEnd);
		highlightedText = ToRichStrWithHighlight(scratch, textbox->edit.str, selectionMin, selectionMax);
	}
	
	SetTextBackgroundColor(GetThemeColor(OptionListBack));
	DrawRichTextWithFont(clayFont->pntr, textbox->fontSize, clayFont->styleFlags, highlightedText, textbox->textPos, GetThemeColor(OptionOnNameText));
	
	if (textbox->edit.isFocused && textbox->edit.cursorActive)
	{
		r32 lineHeight = GetFontLineHeight(clayFont->pntr, textbox->fontSize, clayFont->styleFlags);
		r32 centerOffset = GetFontCenterOffset(clayFont->pntr, textbox->fontSize, clayFont->styleFlags);
		bool foundCursorPos = false;
		v2 cursorPos = V2_Zero;
		VarArrayLoop(&textbox->flowGlyphs, gIndex)
		{
			VarArrayLoopGet(FontFlowGlyph, glyph, &textbox->flowGlyphs, gIndex);
			u8 glyphByteSize = GetCodepointUtf8Size(glyph->codepoint);
			if (glyph->byteIndex + glyphByteSize > textbox->edit.cursorEnd)
			{
				foundCursorPos = true;
				cursorPos = Add(textbox->textPos, glyph->position);
				break;
			}
			else if (glyph->glyph != nullptr && glyph->byteIndex + glyphByteSize == textbox->edit.cursorEnd)
			{
				foundCursorPos = true;
				cursorPos = Add(textbox->textPos, Add(glyph->position, MakeV2(glyph->glyph->metrics.advanceX, 0)));
				break;
			}
		}
		if (!foundCursorPos)
		{
			foundCursorPos = true;
			cursorPos = textbox->textPos;
		}
		
		rec cursorRec = MakeRec(
			cursorPos.X,
			cursorPos.Y - centerOffset - lineHeight/2.0f,
			1.0f,
			lineHeight
		);
		AlignV2(&cursorRec.TopLeft);
		
		Color32 cursorColor = GetThemeColor(OptionOnNameText);
		cursorColor.a = (u8)RoundR32i(OscillateBy(appIn->programTime, 0, 255, 1000, textbox->edit.cursorMoveTime + 600));
		
		DrawRectangle(cursorRec, cursorColor);
	}
	
	SetClipRec(oldClipRec);
	ScratchEnd(scratch);
}
#endif
