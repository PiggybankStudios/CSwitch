/*
File:   app_theme.c
Author: Taylor Robbins
Date:   12\10\2025
Description: 
	** TODO: Update this description once we settle fully on a system for themes!
	** Holds functions related to interacting with Theme structures which holds a set of colors and override booleans for each slot
	** Theme structures are used to store colors that we use for UI elements. It's not a complex structure but it allows us to let the user pick from multiple themes (primarily "light" and "dark" themes)
*/

//NOTE: This is the primary usage macro in the UI code. See the Theme_XList macro for possible names
#define GetThemeColorEx(themeColorSuffix, themeState) (app->theme.entries[ThemeColor_##themeColorSuffix].colors[themeState])
#define GetThemeColor(themeColorSuffix) GetThemeColorEx(themeColorSuffix, ThemeState_Default)

void FreeThemeDefEntry(ThemeDefinition* theme, ThemeDefEntry* entry)
{
	NotNull(theme);
	NotNull(theme->arena);
	if (entry->isStrValue && CanArenaFree(theme->arena))
	{
		FreeStr8(theme->arena, &entry->valueStr);
	}
	ClearPointer(entry);
}
void FreeThemeDefinition(ThemeDefinition* theme)
{
	NotNull(theme);
	if (theme->arena != nullptr)
	{
		VarArrayLoop(&theme->entries, eIndex)
		{
			VarArrayLoopGet(ThemeDefEntry, entry, &theme->entries, eIndex);
			FreeThemeDefEntry(theme, entry);
		}
		FreeVarArray(&theme->entries);
	}
	ClearPointer(theme);
}
void InitThemeDefinition(Arena* arena, ThemeDefinition* themeOut, uxx numEntriesExpected)
{
	NotNull(arena);
	NotNull(themeOut);
	ClearPointer(themeOut);
	themeOut->arena = arena;
	InitVarArrayWithInitial(ThemeDefEntry, &themeOut->entries, arena, numEntriesExpected);
}

void CombineThemeDefinitions(const ThemeDefinition* baseTheme, const ThemeDefinition* overrideTheme, ThemeDefinition* themeOut)
{
	NotNull(baseTheme);
	NotNull(overrideTheme);
	NotNull(themeOut);
	NotNull(themeOut->arena);
	
	VarArrayLoop(&baseTheme->entries, eIndex)
	{
		const VarArrayLoopGet(ThemeDefEntry, baseEntry, &baseTheme->entries, eIndex);
		if (baseEntry->isStrValue)
		{
			AddThemeDefEntryStr(themeOut, baseEntry->mode, baseEntry->state, baseEntry->key, baseEntry->valueStr);
		}
		else
		{
			AddThemeDefEntryColor(themeOut, baseEntry->mode, baseEntry->state, baseEntry->key, baseEntry->valueColor);
		}
	}
	VarArrayLoop(&overrideTheme->entries, eIndex)
	{
		const VarArrayLoopGet(ThemeDefEntry, overrideEntry, &overrideTheme->entries, eIndex);
		if (overrideEntry->isStrValue)
		{
			AddThemeDefEntryStr(themeOut, overrideEntry->mode, overrideEntry->state, overrideEntry->key, overrideEntry->valueStr);
		}
		else
		{
			AddThemeDefEntryColor(themeOut, overrideEntry->mode, overrideEntry->state, overrideEntry->key, overrideEntry->valueColor);
		}
	}
}

Result BakeTheme(ThemeDefinition* themeDef, ThemeMode mode, BakedTheme* themeOut)
{
	Result result = Result_None;
	
	if (mode == ThemeMode_Debug)
	{
		for (uxx cIndex = 1; cIndex < ThemeColor_Count; cIndex++)
		{
			for (uxx sIndex = 0; sIndex < ThemeState_Count; sIndex++)
			{
				themeOut->entries[cIndex].colors[sIndex] = GetPredefPalColorByIndex(cIndex + sIndex);
			}
		}
		return Result_Success;
	}
	
	VarArrayLoop(&themeDef->entries, eIndex)
	{
		VarArrayLoopGet(ThemeDefEntry, entry, &themeDef->entries, eIndex);
		entry->isResolved = false;
		entry->isReferenced = false;
	}
	
	uxx numUnresolvedEntries = 1;
	uxx resolveIter = 0;
	while (numUnresolvedEntries > 0)
	{
		numUnresolvedEntries = 0;
		ThemeDefEntry* firstUnresolvedEntry = nullptr;
		uxx numNewlyResolvedEntries = 0;
		VarArrayLoop(&themeDef->entries, eIndex)
		{
			VarArrayLoopGet(ThemeDefEntry, entry, &themeDef->entries, eIndex);
			if (!entry->isResolved)
			{
				if (!entry->isStrValue)
				{
					entry->isResolved = true;
					numNewlyResolvedEntries++;
				}
				else
				{
					Str8 strippedReferenceName = entry->valueStr;
					ThemeState referencedState = ThemeState_Any;
					for (uxx sIndex = 1; sIndex < ThemeState_Count; sIndex++)
					{
						ThemeState possibleThemeState = (ThemeState)sIndex;
						Str8 themeStateName = MakeStr8Nt(GetThemeStateStr(possibleThemeState));
						if (strippedReferenceName.length > 1 + themeStateName.length &&
							strippedReferenceName.chars[strippedReferenceName.length - themeStateName.length - 1] == '_' &&
							StrAnyCaseEndsWith(strippedReferenceName, themeStateName))
						{
							strippedReferenceName = StrSlice(strippedReferenceName, 0, strippedReferenceName.length - themeStateName.length - 1);
							referencedState = possibleThemeState;
							break;
						}
					}
					
					ThemeDefEntry* referencedEntry = FindThemeDefEntry(themeDef, entry->mode, true, referencedState, false, entry->valueStr);
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
		#if (1 && DEBUG_BUILD)
		//Throw warnings for unreferenced variables that don't match a ThemeColor enum value
		VarArrayLoop(&themeDef->entries, eIndex)
		{
			VarArrayLoopGet(ThemeDefEntry, entry, &themeDef->entries, eIndex);
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
			ThemeColor themeColor = (ThemeColor)cIndex;
			u8 colorStateFlags = GetThemeColorStateFlags(themeColor);
			Str8 enumValueName = MakeStr8Nt(GetThemeColorStr(themeColor));
			for (uxx sIndex = 0; sIndex < ThemeState_Count; sIndex++)
			{
				ThemeState themeState = (ThemeState)sIndex;
				if (IsThemeStatePossibleFromFlags(colorStateFlags, themeState))
				{
					ThemeDefEntry* referencedEntry = FindThemeDefEntry(themeDef, mode, true, themeState, true, enumValueName);
					if (referencedEntry != nullptr)
					{
						themeOut->entries[cIndex].colors[sIndex] = referencedEntry->valueColor;
					}
					else
					{
						NotifyPrint_E("Baking Theme that is missing an entry for \"%.*s\" %s", StrPrint(enumValueName), GetThemeStateStr(themeState));
						themeOut->entries[cIndex].colors[sIndex] = Black;
					}
				}
			}
		}
		
		result = Result_Success;
	}
	
	return result;
}

Result TryParseThemeFile(Str8 fileContents, ThemeDefinition* themeOut)
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
				
				uxx numThemeStates = 0;
				ThemeState themeStates[3];
				
				Str8 strippedNamed = token.key;
				while (strippedNamed.length > 0 && numThemeStates < ArrayCount(themeStates))
				{
					bool foundSuffix = false;
					for (uxx sIndex = 1; sIndex < ThemeState_Count; sIndex++)
					{
						ThemeState possibleThemeState = (ThemeState)sIndex;
						Str8 themeStateName = MakeStr8Nt(GetThemeStateStr(possibleThemeState));
						if (strippedNamed.length > 1 + themeStateName.length &&
							strippedNamed.chars[strippedNamed.length - themeStateName.length - 1] == '_' &&
							StrAnyCaseEndsWith(strippedNamed, themeStateName))
						{
							strippedNamed = StrSlice(strippedNamed, 0, strippedNamed.length - themeStateName.length - 1);
							themeStates[numThemeStates] = possibleThemeState;
							numThemeStates++;
							foundSuffix = true;
							break;
						}
					}
					if (!foundSuffix) { break; }
				}
				
				if (numThemeStates == 0)
				{
					themeStates[0] = ThemeState_Any;
					numThemeStates = 1;
				}
				
				Color32 colorValue = Black;
				if (TryParseColor(token.value, &colorValue, nullptr))
				{
					for (uxx sIndex = 0; sIndex < numThemeStates; sIndex++)
					{
						isNewEntry = AddThemeDefEntryColor(themeOut, currentMode, themeStates[sIndex], strippedNamed, colorValue);
						if (!isNewEntry)
						{
							NotifyPrint_W("Duplicate entry in theme file for \"%.*s\"%s%s%s on line %llu",
								StrPrint(strippedNamed),
								(themeStates[sIndex] == ThemeState_Any ? "" : " ("),
								(themeStates[sIndex] == ThemeState_Any ? "" : GetThemeStateStr(themeStates[sIndex])),
								(themeStates[sIndex] == ThemeState_Any ? "" : ")"),
								parser.lineParser.lineIndex
							);
						}
					}
				}
				else
				{
					for (uxx sIndex = 0; sIndex < numThemeStates; sIndex++)
					{
						isNewEntry = AddThemeDefEntryStr(themeOut, currentMode, themeStates[sIndex], strippedNamed, token.value);
						if (!isNewEntry)
						{
							NotifyPrint_W("Duplicate entry in theme file for \"%.*s\"%s%s%s on line %llu",
								StrPrint(strippedNamed),
								(themeStates[sIndex] == ThemeState_Any ? "" : " ("),
								(themeStates[sIndex] == ThemeState_Any ? "" : GetThemeStateStr(themeStates[sIndex])),
								(themeStates[sIndex] == ThemeState_Any ? "" : ")"),
								parser.lineParser.lineIndex
							);
						}
					}
				}
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

Result TryLoadThemeFile(FilePath filePath, ThemeDefinition* themeOut)
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
