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
		AddThemeDefEntryPntr(themeOut, baseEntry);
	}
	VarArrayLoop(&overrideTheme->entries, eIndex)
	{
		const VarArrayLoopGet(ThemeDefEntry, overrideEntry, &overrideTheme->entries, eIndex);
		AddThemeDefEntryPntr(themeOut, overrideEntry);
	}
}

ThemeState StripThemeIdentifierStateSuffix(Str8* identifierPntr)
{
	for (uxx sIndex = 1; sIndex < ThemeState_Count; sIndex++)
	{
		ThemeState possibleThemeState = (ThemeState)sIndex;
		Str8 themeStateName = MakeStr8Nt(GetThemeStateStr(possibleThemeState));
		if (identifierPntr->length > 1 + themeStateName.length &&
			identifierPntr->chars[identifierPntr->length - themeStateName.length - 1] == '_' &&
			StrAnyCaseEndsWith(*identifierPntr, themeStateName))
		{
			*identifierPntr = StrSlice(*identifierPntr, 0, identifierPntr->length - themeStateName.length - 1);
			return possibleThemeState;
		}
	}
	return ThemeState_Any;
}

Result TryResolveThemeIdentifier(const ThemeDefinition* themeDef, ThemeMode mode, Str8 identifier, Color32* colorOut)
{
	ThemeState stateSuffix = StripThemeIdentifierStateSuffix(&identifier);
	const ThemeDefEntry* referencedEntry = FindThemeDefEntry((ThemeDefinition*)themeDef, mode, true, stateSuffix, true, identifier);
	while (referencedEntry != nullptr)
	{
		if (referencedEntry->type == ThemeDefEntryType_Color || referencedEntry->isResolved)
		{
			SetOptionalOutPntr(colorOut, referencedEntry->color);
			return Result_Success;
		}
		else if (referencedEntry->type == ThemeDefEntryType_Reference)
		{
			Str8 referenceIdentifier = referencedEntry->referenceKey;
			stateSuffix = StripThemeIdentifierStateSuffix(&referenceIdentifier);
			referencedEntry = FindThemeDefEntry((ThemeDefinition*)themeDef, mode, true, stateSuffix, true, referenceIdentifier);
		}
		else { return Result_Unresolved; }
	}
	return Result_NotFound;
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
	uxx resolveIter = 0; UNUSED(resolveIter);
	while (numUnresolvedEntries > 0)
	{
		numUnresolvedEntries = 0;
		ThemeDefEntry* firstUnresolvedEntry = nullptr;
		uxx numNewlyResolvedEntries = 0;
		VarArrayLoop(&themeDef->entries, eIndex)
		{
			VarArrayLoopGet(ThemeDefEntry, entry, &themeDef->entries, eIndex);
			if (!entry->isResolved && (entry->mode == mode || entry->mode == ThemeMode_None))
			{
				if (entry->type == ThemeDefEntryType_Color)
				{
					entry->isResolved = true;
					numNewlyResolvedEntries++;
				}
				else if (entry->type == ThemeDefEntryType_Reference)
				{
					Str8 strippedReferenceName = entry->referenceKey;
					ThemeState referencedState = StripThemeIdentifierStateSuffix(&strippedReferenceName);
					ThemeDefEntry* referencedEntry = FindThemeDefEntry(themeDef, entry->mode, true, referencedState, false, strippedReferenceName);
					if (referencedEntry != nullptr)
					{
						referencedEntry->isReferenced = true;
						if (referencedEntry->isResolved)
						{
							entry->isResolved = true;
							entry->color = referencedEntry->color;
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
						NotifyPrint_E("Unknown identifier: \"%.*s\" given for theme value file for \"%.*s\"!", StrPrint(entry->referenceKey), StrPrint(entry->key));
						result = Result_UnknownString;
					}
				}
				else if (entry->type == ThemeDefEntryType_Function)
				{
					bool allArgsResolved = true;
					ThemeDefFuncArgValue resolvedArgs[THEME_DEF_FUNC_MAX_ARGS];
					for (uxx aIndex = 0; aIndex < entry->functionArgCount; aIndex++)
					{
						if (entry->functionArgValues[aIndex].type == ThemeDefFuncArgType_Identifier)
						{
							Color32 resolvedColor = Transparent_Const;
							Result resolveResult = TryResolveThemeIdentifier(themeDef, mode, entry->functionArgValues[aIndex].valueIdentifier, &resolvedColor);
							if (resolveResult == Result_Success)
							{
								resolvedArgs[aIndex].type = ThemeDefFuncArgType_Color;
								resolvedArgs[aIndex].valueColor = resolvedColor;
							}
							else { allArgsResolved = false; break; }
						}
						else
						{
							MyMemCopy(&resolvedArgs[aIndex], &entry->functionArgValues[aIndex], sizeof(ThemeDefFuncArgValue));
						}
					}
					
					if (allArgsResolved)
					{
						Color32 functionResult = CallThemeDefFunc(
							entry->function,
							themeDef, mode,
							entry->functionArgCount, &resolvedArgs[0]
						);
						entry->isResolved = true;
						entry->color = functionResult;
						numNewlyResolvedEntries++;
					}
					else
					{
						numUnresolvedEntries++;
					}
				}
				else { Assert(false); }
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
			if (!entry->isReferenced && (entry->mode == mode || entry->mode == ThemeMode_None))
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
						themeOut->entries[cIndex].colors[sIndex] = referencedEntry->color;
					}
					else
					{
						NotifyPrint_E("Baked Theme is missing an entry for \"%.*s\" %s", StrPrint(enumValueName), GetThemeStateStr(themeState));
						themeOut->entries[cIndex].colors[sIndex] = Black;
						result = Result_MissingItems;
						break;
					}
				}
			}
			if (result != Result_None) { break; }
		}
		
		if (result == Result_None) { result = Result_Success; }
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
				Str8 strippedName = token.key;
				while (strippedName.length > 0 && numThemeStates < ArrayCount(themeStates))
				{
					ThemeState strippedStateSuffix = StripThemeIdentifierStateSuffix(&strippedName);
					if (strippedStateSuffix != ThemeState_Any)
					{
						themeStates[numThemeStates] = strippedStateSuffix;
						numThemeStates++;
					}
					else { break; }
				}
				if (numThemeStates == 0)
				{
					themeStates[0] = ThemeState_Any;
					numThemeStates = 1;
				}
				
				// +==============================+
				// |      Parse Color Entry       |
				// +==============================+
				Color32 colorValue = Black;
				if (TryParseColor(token.value, &colorValue, nullptr))
				{
					for (uxx sIndex = 0; sIndex < numThemeStates; sIndex++)
					{
						isNewEntry = AddThemeDefEntryColor(themeOut, currentMode, themeStates[sIndex], strippedName, colorValue);
						if (!isNewEntry)
						{
							NotifyPrint_W("Duplicate entry in theme file for \"%.*s\"%s%s%s on line %llu",
								StrPrint(strippedName),
								(themeStates[sIndex] == ThemeState_Any ? "" : " ("),
								(themeStates[sIndex] == ThemeState_Any ? "" : GetThemeStateStr(themeStates[sIndex])),
								(themeStates[sIndex] == ThemeState_Any ? "" : ")"),
								parser.lineParser.lineIndex
							);
						}
					}
				}
				// +==============================+
				// |     Parse Function Entry     |
				// +==============================+
				else if (FindNextUnknownCharInStr(token.value, 0, StrLit(IDENTIFIER_CHARS)) < token.value.length)
				{
					bool parsedFunction = false;
					
					for (uxx fIndex = 1; fIndex < ThemeDefFunc_Count; fIndex++)
					{
						ThemeDefFunc function = (ThemeDefFunc)fIndex;
						Str8 functionName = MakeStr8Nt(GetThemeDefFuncStr(function));
						if (StrAnyCaseStartsWith(token.value, functionName))
						{
							ThemeDefFuncArgInfo funcArgInfo = GetThemeDefFuncArgInfo(function);
							
							Str8 argsStr = TrimWhitespace(StrSliceFrom(token.value, functionName.length));
							if (argsStr.length >= 2 && argsStr.chars[0] == '(' && argsStr.chars[argsStr.length-1] == ')')
							{
								argsStr = TrimWhitespace(StrSlice(argsStr, 1, argsStr.length-1));
							}
							
							uxx argIndex = 0;
							ThemeDefFuncArgValue argValues[THEME_DEF_FUNC_MAX_ARGS];
							bool parsedAllArguments = true;
							uxx prevCommaIndex = 0;
							for (uxx cIndex = 0; cIndex <= argsStr.length; cIndex++)
							{
								if (cIndex == argsStr.length || argsStr.chars[cIndex] == ',')
								{
									Str8 argStr = TrimWhitespace(StrSlice(argsStr, prevCommaIndex, cIndex));
									if (argStr.length == 0) { return Result_EmptyArgument; }
									if (argIndex >= funcArgInfo.argCount)
									{
										PrintLine_W("Too many arguments for function %.*s (expected %llu) on line %llu",
											StrPrint(functionName),
											funcArgInfo.argCount,
											parser.lineParser.lineIndex
										);
										parsedAllArguments = false;
										break;
									}
									
									bool argParseSuccess = false;
									Result argParseError = Result_None;
									ThemeDefFuncArgType actualArgType = funcArgInfo.argType[argIndex];
									switch (funcArgInfo.argType[argIndex])
									{
										case ThemeDefFuncArgType_U8: argParseSuccess = TryParseU8(argStr, &argValues[argIndex].valueU8, &argParseError); break;
										case ThemeDefFuncArgType_I32: argParseSuccess = TryParseI32(argStr, &argValues[argIndex].valueI32, &argParseError); break;
										case ThemeDefFuncArgType_R32: argParseSuccess = TryParseR32(argStr, &argValues[argIndex].valueR32, &argParseError); break;
										case ThemeDefFuncArgType_Color:
										{
											argParseSuccess = TryParseColor(argStr, &argValues[argIndex].valueColor, &argParseError);
											
											//NOTE: Color type parameters could be identifiers that get resolved during the baking process
											if (!argParseSuccess && IsValidIdentifier(argStr.length, argStr.chars, false, false, false))
											{
												actualArgType = ThemeDefFuncArgType_Identifier;
												argValues[argIndex].valueIdentifier = argStr;
												argParseSuccess = true;
											}
										} break;
										case ThemeDefFuncArgType_Identifier:
										{
											if (IsValidIdentifier(argStr.length, argStr.chars, false, false, false))
											{
												argValues[argIndex].valueIdentifier = argStr;
												argParseSuccess = true;
											}
											else
											{
												argParseError = Result_InvalidIdentifier;
												argParseSuccess = false;
											}
											
										} break;
										default: Assert(false); break;
									}
									
									if (argParseSuccess)
									{
										argValues[argIndex].type = actualArgType;
									}
									else
									{
										PrintLine_W("Couldn't parse function %.*s arg %llu as %s on line %llu: \"%.*s\" (Error: %s)",
											StrPrint(functionName),
											argIndex,
											GetThemeDefFuncArgTypeStr(actualArgType),
											parser.lineParser.lineIndex,
											StrPrint(argStr),
											GetResultStr(argParseError)
										);
										parsedAllArguments = false;
										break;
									}
									
									prevCommaIndex = cIndex+1;
									argIndex++;
								}
							}
							
							if (parsedAllArguments && argIndex == GetThemeDefFuncArgCount(function))
							{
								for (uxx sIndex = 0; sIndex < numThemeStates; sIndex++)
								{
									isNewEntry = AddThemeDefEntryFunction(themeOut, currentMode, themeStates[sIndex], strippedName, function, argIndex, &argValues[0]);
									if (!isNewEntry)
									{
										NotifyPrint_W("Duplicate entry in theme file for \"%.*s\"%s%s%s on line %llu",
											StrPrint(strippedName),
											(themeStates[sIndex] == ThemeState_Any ? "" : " ("),
											(themeStates[sIndex] == ThemeState_Any ? "" : GetThemeStateStr(themeStates[sIndex])),
											(themeStates[sIndex] == ThemeState_Any ? "" : ")"),
											parser.lineParser.lineIndex
										);
									}
								}
								parsedFunction = true;
								break;
							}
						}
					}
					
					if (!parsedFunction)
					{
						NotifyPrint_E("Invalid function in theme file for \"%.*s\" on line %llu: \"%.*s\"",
							StrPrint(strippedName),
							parser.lineParser.lineIndex,
							StrPrint(token.value)
						);
						return Result_InvalidSyntax;
					}
				}
				// +==============================+
				// |    Parse Reference Entry     |
				// +==============================+
				else
				{
					for (uxx sIndex = 0; sIndex < numThemeStates; sIndex++)
					{
						isNewEntry = AddThemeDefEntryReference(themeOut, currentMode, themeStates[sIndex], strippedName, token.value);
						if (!isNewEntry)
						{
							NotifyPrint_W("Duplicate entry in theme file for \"%.*s\"%s%s%s on line %llu",
								StrPrint(strippedName),
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
