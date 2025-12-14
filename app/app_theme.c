/*
File:   app_theme.c
Author: Taylor Robbins
Date:   12\10\2025
Description: 
	** Holds functions related to interacting with Theme structures which holds a set of colors and override booleans for each slot
	** Theme structures are used to store colors that we use for UI elements. It's not a complex structure but it allows us to let the user pick from multiple themes (primarily "light" and "dark" themes)
*/

//NOTE: This is the primary usage macro in the UI code. See the Theme_XList macro for possible names
#define GetThemeColor(themeColorSuffix) (app->theme.colors[ThemeColor_##themeColorSuffix])

void FreeUserThemeEntry(UserTheme* theme, UserThemeEntry* entry)
{
	NotNull(theme);
	NotNull(theme->arena);
	if (entry->isStrValue && CanArenaFree(theme->arena))
	{
		FreeStr8(theme->arena, &entry->valueStr);
	}
	ClearPointer(entry);
}
void FreeUserTheme(UserTheme* theme)
{
	NotNull(theme);
	if (theme->arena != nullptr)
	{
		VarArrayLoop(&theme->entries, eIndex)
		{
			VarArrayLoopGet(UserThemeEntry, entry, &theme->entries, eIndex);
			FreeUserThemeEntry(theme, entry);
		}
		FreeVarArray(&theme->entries);
	}
	ClearPointer(theme);
}
void InitUserTheme(Arena* arena, UserTheme* themeOut, uxx numEntriesExpected)
{
	NotNull(arena);
	NotNull(themeOut);
	ClearPointer(themeOut);
	themeOut->arena = arena;
	InitVarArrayWithInitial(UserThemeEntry, &themeOut->entries, arena, numEntriesExpected);
}

void CombineUserTheme(const UserTheme* baseTheme, const UserTheme* overrideTheme, UserTheme* themeOut)
{
	NotNull(baseTheme);
	NotNull(overrideTheme);
	NotNull(themeOut);
	NotNull(themeOut->arena);
	
	VarArrayLoop(&baseTheme->entries, eIndex)
	{
		const VarArrayLoopGet(UserThemeEntry, baseEntry, &baseTheme->entries, eIndex);
		if (baseEntry->isStrValue)
		{
			AddUserThemeEntryStr(themeOut, baseEntry->mode, baseEntry->key, baseEntry->valueStr);
		}
		else
		{
			AddUserThemeEntryColor(themeOut, baseEntry->mode, baseEntry->key, baseEntry->valueColor);
		}
	}
	VarArrayLoop(&overrideTheme->entries, eIndex)
	{
		const VarArrayLoopGet(UserThemeEntry, overrideEntry, &overrideTheme->entries, eIndex);
		if (overrideEntry->isStrValue)
		{
			AddUserThemeEntryStr(themeOut, overrideEntry->mode, overrideEntry->key, overrideEntry->valueStr);
		}
		else
		{
			AddUserThemeEntryColor(themeOut, overrideEntry->mode, overrideEntry->key, overrideEntry->valueColor);
		}
	}
}

Result BakeUserTheme(UserTheme* userTheme, ThemeMode mode, BakedTheme* themeOut)
{
	Result result = Result_None;
	
	VarArrayLoop(&userTheme->entries, eIndex)
	{
		VarArrayLoopGet(UserThemeEntry, entry, &userTheme->entries, eIndex);
		entry->isResolved = false;
		entry->isReferenced = false;
	}
	
	uxx numUnresolvedEntries = 1;
	uxx resolveIter = 0;
	while (numUnresolvedEntries > 0)
	{
		numUnresolvedEntries = 0;
		UserThemeEntry* firstUnresolvedEntry = nullptr;
		uxx numNewlyResolvedEntries = 0;
		VarArrayLoop(&userTheme->entries, eIndex)
		{
			VarArrayLoopGet(UserThemeEntry, entry, &userTheme->entries, eIndex);
			if (!entry->isResolved)
			{
				if (!entry->isStrValue)
				{
					entry->isResolved = true;
					numNewlyResolvedEntries++;
				}
				else
				{
					UserThemeEntry* referencedEntry = FindUserThemeEntry(userTheme, entry->mode, entry->valueStr);
					if (referencedEntry != nullptr)
					{
						referencedEntry->isReferenced = true;
						if (referencedEntry->isResolved)
						{
							entry->isResolved = true;
							entry->valueColor = referencedEntry->valueColor;
							numNewlyResolvedEntries++;
						}
						else
						{
							if (firstUnresolvedEntry == nullptr) { firstUnresolvedEntry = entry; }
							numUnresolvedEntries++;
						}
					}
					else
					{
						NotifyPrint_E("Unknown identifier: \"%.*s\" given for theme value file for \"%.*s\"!", StrPrint(entry->valueStr), StrPrint(entry->key));
						result = Result_UnknownString;
					}
				}
			}
		}
		if (numNewlyResolvedEntries == 0 && numUnresolvedEntries > 0)
		{
			NotifyPrint_E("Unresolved entries in theme (circular dependencies between identifiers). Unresolved entries \"%.*s\" and %llu other%s",
				StrPrint(firstUnresolvedEntry->key),
				(numUnresolvedEntries-1), Plural(numUnresolvedEntries-1, "s")
			);
			result = Result_InvalidInput;
			break;
		}
		resolveIter++;
	}
	
	if (numUnresolvedEntries == 0 && result == Result_None)
	{
		#if (0 && DEBUG_BUILD)
		//Throw warnings for unreferenced variables that don't match a ThemeColor enum value
		VarArrayLoop(&userTheme->entries, eIndex)
		{
			VarArrayLoopGet(UserThemeEntry, entry, &userTheme->entries, eIndex);
			if (!entry->isReferenced)
			{
				for (uxx cIndex = 1; cIndex < ThemeColor_Count; cIndex++)
				{
					Str8 enumValueName = MakeStr8Nt(GetThemeColorStr((ThemeColor)cIndex));
					if (StrExactEquals(entry->key, enumValueName)) { entry->isReferenced = true; break; }
				}
				if (!entry->isReferenced)
				{
					NotifyPrint_W("Unreferenced variable in theme: \"%.*s\". Is this a typo?", StrPrint(entry->key));
				}
			}
		}
		#endif
		
		for (uxx cIndex = 1; cIndex < ThemeColor_Count; cIndex++)
		{
			Str8 enumValueName = MakeStr8Nt(GetThemeColorStr((ThemeColor)cIndex));
			UserThemeEntry* referencedEntry = FindUserThemeEntry(userTheme, mode, enumValueName);
			if (referencedEntry != nullptr)
			{
				themeOut->colors[cIndex] = referencedEntry->valueColor;
			}
			else
			{
				PrintLine_E("Baking UserTheme that is missing an entry for \"%.*s\"", StrPrint(enumValueName));
				DebugAssertMsg(referencedEntry != nullptr, "Baking incomplete theme! Missing entry for one of the ThemeColor values!");
				themeOut->colors[cIndex] = Black;
			}
		}
		
		result = Result_Success;
	}
	
	return result;
}

Result TryParseThemeFile(Str8 fileContents, UserTheme* themeOut)
{
	NotNull(themeOut);
	NotNull(themeOut->arena);
	if (IsEmptyStr(fileContents)) { return Result_EmptyFile; }
	
	ThemeMode currentMode = ThemeMode_None;
	
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
				ThemeMode themeMode = ThemeMode_None;
				if (TryParseThemeMode(token.value, &themeMode) && themeMode != ThemeMode_None)
				{
					currentMode = themeMode;
				}
				else if (StrAnyCaseEquals(token.value, StrLit("All")))
				{
					currentMode = ThemeMode_None;
				}
				else
				{
					NotifyPrint_E("Unknown preset name in theme file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.value));
					return Result_InvalidSyntax;
				}
			} break;
			
			case ParsingTokenType_KeyValuePair:
			{
				bool isNewEntry = false;
				
				Color32 colorValue = Black;
				if (TryParseColor(token.value, &colorValue, nullptr))
				{
					isNewEntry = AddUserThemeEntryColor(themeOut, currentMode, token.key, colorValue);
				}
				else
				{
					isNewEntry = AddUserThemeEntryStr(themeOut, currentMode, token.key, token.value);
				}
				
				if (!isNewEntry) { NotifyPrint_W("Duplicate entry in theme file for \"%.*s\" on line %llu", StrPrint(token.str), parser.lineParser.lineIndex); }
			} break;
			
			default:
			{
				NotifyPrint_W("Invalid syntax in theme file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.str));
				return Result_InvalidSyntax;
			} break;
		}
	}
	
	return Result_Success;
}

Result TryLoadThemeFile(FilePath filePath, UserTheme* themeOut)
{
	NotNull(themeOut);
	NotNull(themeOut->arena);
	ScratchBegin1(scratch, themeOut->arena);
	Str8 fileContents = Str8_Empty;
	if (!OsReadTextFile(filePath, scratch, &fileContents))
	{
		ScratchEnd(scratch);
		return Result_FailedToReadFile;
	}
	Result result = TryParseThemeFile(fileContents, themeOut);
	ScratchEnd(scratch);
	return result;
}

//TODO: How do we want to do serialization since a single Theme structure cannot hold overrides for multiple presets but the file format can?
//      Maybe the calling code should pass us multiple Theme structures? Or maybe it should stitch multiple serializations together?
// Str8 SerializeThemeFile(Arena* arena, Theme* theme)
// {
// }
