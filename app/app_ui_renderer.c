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
					
					if (cmd->rectangle.borderThickness.Left > 0.0f || cmd->rectangle.borderThickness.Top > 0.0f ||
						cmd->rectangle.borderThickness.Right > 0.0f || cmd->rectangle.borderThickness.Bottom > 0.0f)
					{
						DrawRectangleOutlineSidesEx(
							cmd->rectangle.rectangle,
							cmd->rectangle.borderThickness.Left, cmd->rectangle.borderThickness.Right, cmd->rectangle.borderThickness.Top, cmd->rectangle.borderThickness.Bottom,
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
							cmd->rectangle.cornerRadius.TopLeft, cmd->rectangle.cornerRadius.TopRight, cmd->rectangle.cornerRadius.BottomRight, cmd->rectangle.cornerRadius.BottomLeft,
							cmd->color
						);
					}
					else
					{
						DrawTexturedRoundedRectangleEx(cmd->rectangle.rectangle,
							cmd->rectangle.cornerRadius.TopLeft, cmd->rectangle.cornerRadius.TopRight, cmd->rectangle.cornerRadius.BottomRight, cmd->rectangle.cornerRadius.BottomLeft,
							cmd->color,
							cmd->rectangle.texture,
							cmd->rectangle.sourceRec
						);
					}
					
					r32 borderThickness = MaxR32(MaxR32(cmd->rectangle.borderThickness.X, cmd->rectangle.borderThickness.Y), MaxR32(cmd->rectangle.borderThickness.Z, cmd->rectangle.borderThickness.W));
					if (borderThickness && cmd->rectangle.borderColor.a > 0)
					{
						DrawTexturedRoundedRectangleOutlineEx(
							cmd->rectangle.rectangle,
							borderThickness,
							cmd->rectangle.cornerRadius.TopLeft, cmd->rectangle.cornerRadius.TopRight, cmd->rectangle.cornerRadius.BottomRight, cmd->rectangle.cornerRadius.BottomLeft,
							cmd->rectangle.borderColor,
							/*outside=*/false,
							nullptr,
							Rec_Zero
						);
					}
				}
			} break;
			
			case UiRenderCmdType_Text:
			{
				RichStr richStr = ToRichStr(cmd->text.text);
				v2 textPos = cmd->text.position;
				
				// NOTE: TextContraction_ClipRight is the default behavior
				if (cmd->params.textContraction == TextContraction_ClipLeft)
				{
					TextMeasure textMeasure = MeasureRichTextEx(cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle, true, cmd->text.wrapWidth, richStr);
					if (textMeasure.Width > cmd->clipRec.Width)
					{
						textPos.X -= (textMeasure.Width - cmd->clipRec.Width);
					}
				}
				else if (cmd->params.textContraction == TextContraction_EllipseLeft)
				{
					Str8 text = ShortenTextStartToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->clipRec.Width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				else if (cmd->params.textContraction == TextContraction_EllipseMiddle)
				{
					Str8 text = ShortenTextToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->clipRec.Width),
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
						CeilR32(cmd->clipRec.Width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				else if (cmd->params.textContraction == TextContraction_EllipseFilePath)
				{
					Str8 text = ShortenFilePathToFitWidth(scratch,
						cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle,
						cmd->text.text,
						CeilR32(cmd->clipRec.Width),
						StrLit(UNICODE_ELLIPSIS_STR)
					);
					richStr = ToRichStr(text);
				}
				DrawWrappedRichTextWithFont(cmd->text.font, cmd->text.fontSize, cmd->text.fontStyle, richStr, textPos, cmd->text.wrapWidth, cmd->color);
			} break;
			
			case UiRenderCmdType_RichText:
			{
				DrawWrappedRichTextWithFont(cmd->richText.font, cmd->richText.fontSize, cmd->richText.fontStyle, cmd->richText.text, cmd->richText.position, cmd->richText.wrapWidth, cmd->color);
			} break;
		}
		
		ScratchEnd(scratch);
	}
	DisableClipRec();
	
}

#endif
