/*
File:   app_ui_renderer.c
Author: Taylor Robbins
Date:   06\15\2026
Description: 
	** Holds a function that renders a UiRenderList from Pig UI
*/

#if BUILD_WITH_PIG_UI

void RenderPigUi(UiRenderList* renderList)
{
	VarArrayLoop(&renderList->commands, cIndex)
	{
		VarArrayLoopGet(UiRenderCmd, cmd, &renderList->commands, cIndex);
		ScratchBegin(scratch);
		SetClipRec(ToReciFromf(cmd->clipRec));
		switch (cmd->type)
		{
			// +==============================+
			// |  UiRenderCmdType_Rectangle   |
			// +==============================+
			case UiRenderCmdType_Rectangle:
			{
				if (AreEqualV4r(cmd->rectangle.cornerRadius, V4r_Zero))
				{
					if (cmd->color.a > 0)
					{
						if (cmd->rectangle.texture != nullptr)
						{
							DrawTexturedRectangleEx(cmd->rectangle.rectangle, cmd->color, cmd->rectangle.texture, cmd->rectangle.sourceRec);
						}
						else
						{
							DrawRectangle(cmd->rectangle.rectangle, cmd->color);
						}
					}
					
					if (cmd->rectangle.borderThickness.left > 0.0f || cmd->rectangle.borderThickness.top > 0.0f ||
						cmd->rectangle.borderThickness.right > 0.0f || cmd->rectangle.borderThickness.bottom > 0.0f)
					{
						DrawRectangleOutlineSidesEx(
							cmd->rectangle.rectangle,
							cmd->rectangle.borderThickness.left, cmd->rectangle.borderThickness.right, cmd->rectangle.borderThickness.top, cmd->rectangle.borderThickness.bottom,
							cmd->rectangle.borderColor,
							false
						);
					}
				}
				else
				{
					if (cmd->rectangle.texture != nullptr)
					{
						DrawRoundedRectangleEx(cmd->rectangle.rectangle,
							cmd->rectangle.cornerRadius.topLeft, cmd->rectangle.cornerRadius.topRight, cmd->rectangle.cornerRadius.bottomRight, cmd->rectangle.cornerRadius.bottomLeft,
							cmd->color
						);
					}
					else
					{
						DrawTexturedRoundedRectangleEx(cmd->rectangle.rectangle,
							cmd->rectangle.cornerRadius.topLeft, cmd->rectangle.cornerRadius.topRight, cmd->rectangle.cornerRadius.bottomRight, cmd->rectangle.cornerRadius.bottomLeft,
							cmd->color,
							cmd->rectangle.texture,
							cmd->rectangle.sourceRec
						);
					}
					
					r32 borderThickness = MaxR32(MaxR32(cmd->rectangle.borderThickness.x, cmd->rectangle.borderThickness.y), MaxR32(cmd->rectangle.borderThickness.z, cmd->rectangle.borderThickness.w));
					if (borderThickness && cmd->rectangle.borderColor.a > 0)
					{
						DrawTexturedRoundedRectangleOutlineEx(
							cmd->rectangle.rectangle,
							borderThickness,
							cmd->rectangle.cornerRadius.topLeft, cmd->rectangle.cornerRadius.topRight, cmd->rectangle.cornerRadius.bottomRight, cmd->rectangle.cornerRadius.bottomLeft,
							cmd->rectangle.borderColor,
							/*outside=*/false,
							nullptr,
							Rec_Zero
						);
					}
				}
			} break;
			
			// +==============================+
			// |     UiRenderCmdType_Text     |
			// +==============================+
			case UiRenderCmdType_Text:
			{
				RichStr richStr = ToRichStr(cmd->text.text);
				v2 textPos = cmd->text.position;
				
				// NOTE: TextContraction_ClipRight is the default behavior
				if (cmd->params.textContraction == TextContraction_ClipLeft)
				{
					TextMeasure textMeasure = MeasureRichTextEx(cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle, true, cmd->text.wrapWidth, richStr);
					if (textMeasure.width > cmd->text.bounds.width)
					{
						textPos.x -= (textMeasure.width - cmd->text.bounds.width);
					}
				}
				else if (cmd->params.textContraction == TextContraction_EllipseLeft)
				{
					Str8 text = ShortenTextStartToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->text.bounds.width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				else if (cmd->params.textContraction == TextContraction_EllipseMiddle)
				{
					Str8 text = ShortenTextToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->text.bounds.width),
						StrLit(UNICODE_ELLIPSIS_STR),
						cmd->text.text.length/2
					);
					richStr = ToRichStr(text);
				}
				else if (cmd->params.textContraction == TextContraction_EllipseRight)
				{
					Str8 text = ShortenTextEndToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->text.bounds.width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				else if (cmd->params.textContraction == TextContraction_EllipseFilePath)
				{
					Str8 text = ShortenFilePathToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->text.bounds.width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				else
				{
					if (cmd->text.alignment.x != UiAlign_Left)
					{
						TextMeasure textMeasure = MeasureRichTextEx(cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle, true, cmd->text.wrapWidth, richStr);
						if (textMeasure.width < cmd->text.bounds.width)
						{
							textPos.x = (cmd->text.alignment.x == UiAlign_Right)
								? FloorR32(cmd->text.bounds.x + cmd->text.bounds.width - textMeasure.width)
								: RoundR32(cmd->text.bounds.x + cmd->text.bounds.width/2 - textMeasure.width/2);
						}
					}
					//TODO: Should we handle cmd->text.alignment.y?
				}
				DrawWrappedRichTextWithFont(cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle, richStr, textPos, cmd->text.wrapWidth, cmd->color);
			} break;
			
			// +==============================+
			// |   UiRenderCmdType_RichText   |
			// +==============================+
			case UiRenderCmdType_RichText:
			{
				DrawWrappedRichTextWithFont(cmd->richText.font, cmd->richText.fontSize, cmd->richText.fontStyle, cmd->richText.text, cmd->richText.position, cmd->richText.wrapWidth, cmd->color);
			} break;
		}
		// DisableClipRec();
		// DrawRectangleOutline(cmd->clipRec, 2, ColorWithAlpha(GetPureColorByIndex(cIndex), 0.2f));
		
		ScratchEnd(scratch);
	}
	DisableClipRec();
}

#endif
