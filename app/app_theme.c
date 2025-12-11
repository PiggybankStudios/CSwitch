/*
File:   app_theme.c
Author: Taylor Robbins
Date:   12\10\2025
Description: 
	** Holds functions related to interacting with Theme structures which holds a set of colors and override booleans for each slot
	** Theme structures are used to store colors that we use for UI elements. It's not a complex structure but it allows us to let the user pick from multiple themes (primarily "light" and "dark" themes)
*/

#define GetThemeColor(themeColorSuffix) GetColorFromThemeOrDefault(&app->themePresets[app->currentThemePreset], &app->themeOverrides, ThemeColor_##themeColorSuffix)

//NOTE: This only really extracts some of the colors depending on targetPreset and @Light or @Dark labels in the file. Be careful when reserializing this theme since some colors might have been ommitted
Result TryParseThemeFile(Str8 fileContents, PresetTheme targetPreset, Theme* themeOut)
{
	if (IsEmptyStr(fileContents)) { return Result_EmptyFile; }
	
	if (themeOut != nullptr) { ClearPointer(themeOut); }
	PresetTheme currentPreset = PresetTheme_None;
	bool numColorsParsed = 0;
	
	TextParser parser = MakeTextParser(fileContents);
	ParsingToken token;
	while (TextParserGetToken(&parser, &token))
	{
		switch (token.type)
		{
			// case ParsingTokenType_FilePrefix: //Use this syntax in some way?
			case ParsingTokenType_Comment: /* ignore comments */ break;
			
			case ParsingTokenType_Directive:
			{
				PresetTheme presetTheme = PresetTheme_None;
				if (TryParsePresetTheme(token.value, &presetTheme) && presetTheme != PresetTheme_None)
				{
					currentPreset = presetTheme;
				}
				else
				{
					PrintLine_E("Unknown preset name in theme file: \"%.*s\"", StrPrint(token.value));
				}
			} break;
			
			case ParsingTokenType_KeyValuePair:
			{
				ThemeColor themeColor;
				if (TryParseThemeColor(token.key, &themeColor) && themeColor != ThemeColor_None && themeColor < ThemeColor_Count)
				{
					Color32 colorValue = Black;
					if (TryParseColor(token.value, &colorValue, nullptr))
					{
						numColorsParsed++;
						if (themeOut != nullptr && (currentPreset == PresetTheme_None || currentPreset == targetPreset))
						{
							themeOut->colors[themeColor] = colorValue;
							themeOut->overridden[themeColor] = true;
						}
					}
					else
					{
						PrintLine_W("Unable to parse value as color in theme: \"%.*s\"", StrPrint(token.value));
					}
				}
				else
				{
					PrintLine_W("Unknown key in theme file: \"%.*s\"", StrPrint(token.key));
				}
			} break;
			
			default: PrintLine_W("Unhandled token in theme file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.str)); break;
		}
	}
	
	if (numColorsParsed == 0) { return Result_NoMatch; }
	return Result_Success;
}

Result TryLoadThemeFile(FilePath filePath, PresetTheme targetPreset, Theme* themeOut)
{
	ScratchBegin(scratch);
	Str8 fileContents = Str8_Empty;
	if (!OsReadTextFile(filePath, scratch, &fileContents))
	{
		ScratchEnd(scratch);
		return Result_FailedToReadFile;
	}
	Result result = TryParseThemeFile(fileContents, targetPreset, themeOut);
	ScratchEnd(scratch);
	return result;
}

//TODO: How do we want to do serialization since a single Theme structure cannot hold overrides for multiple presets but the file format can?
//      Maybe the calling code should pass us multiple Theme structures? Or maybe it should stitch multiple serializations together?
// Str8 SerializeThemeFile(Arena* arena, Theme* theme)
// {
// }
