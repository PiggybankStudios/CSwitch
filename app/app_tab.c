/*
File:   app_tab.c
Author: Taylor Robbins
Date:   03\10\2025
Description: 
	** Holds functions that pertain to a single tab, or are used when opening\closing tabs
*/

void FreeFileOption(FileOption* option)
{
	NotNull(option);
	FreeStr8(stdHeap, &option->name);
	FreeStr8(stdHeap, &option->abbreviation);
	FreeStr8(stdHeap, &option->valueStr);
	RemoveTooltipRegionById(&app->tooltipRegions, option->tooltipId);
	ClearPointer(option);
}

void FreeFileTab(FileTab* tab)
{
	NotNull(tab);
	FreeStr8(stdHeap, &tab->filePath);
	FreeStr8(stdHeap, &tab->fileContents);
	FreeStr8(stdHeap, &tab->originalFileContents);
	VarArrayLoop(&tab->fileOptions, oIndex)
	{
		VarArrayLoopGet(FileOption, option, &tab->fileOptions, oIndex);
		FreeFileOption(option);
	}
	FreeVarArray(&tab->fileOptions);
	ClearPointer(tab);
}

void AppCloseFileTab(uxx tabIndex)
{
	Assert(tabIndex < app->tabs.length);
	FileTab* closedTab = VarArrayGetHard(FileTab, &app->tabs, tabIndex);
	
	RemoveFileWatch(&app->fileWatches, closedTab->fileWatchId);
	FreeFileTab(closedTab);
	VarArrayRemoveAt(FileTab, &app->tabs, tabIndex);
	
	if (app->tabs.length == 0)
	{
		app->currentTabIndex = 0;
		app->currentTab = nullptr;
	}
	else if (app->currentTabIndex == tabIndex)
	{
		if (app->currentTabIndex >= app->tabs.length)
		{
			app->currentTabIndex = app->tabs.length-1;
		}
	}
	else if (app->currentTabIndex > tabIndex)
	{
		app->currentTabIndex--;
	}
	
	app->currentTab = VarArrayGetSoft(FileTab, &app->tabs, app->currentTabIndex);
	if (app->currentTab != nullptr)
	{
		platform->SetWindowTitle(ScratchPrintStr("%.*s - %s", StrPrint(app->currentTab->filePath), PROJECT_READABLE_NAME_STR));
	}
	else
	{
		platform->SetWindowTitle(StrLit(PROJECT_READABLE_NAME_STR));
	}
}

FileTab* AppFindTabForPath(FilePath filePath)
{
	ScratchBegin(scratch);
	Str8 fullPath = OsGetFullPath(scratch, filePath);
	VarArrayLoop(&app->tabs, tIndex)
	{
		VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
		Str8 tabFullPath = OsGetFullPath(scratch, tab->filePath);
		if (StrAnyCaseEquals(tabFullPath, fullPath))
		{
			ScratchEnd(scratch);
			return tab;
		}
	}
	ScratchEnd(scratch);
	return nullptr;
}

void AppChangeTab(uxx newTabIndex)
{
	if (app->currentTabIndex == newTabIndex && app->currentTab != nullptr) { return; }
	Assert(newTabIndex < app->tabs.length);
	app->currentTabIndex = newTabIndex;
	app->currentTab = VarArrayGetHard(FileTab, &app->tabs, app->currentTabIndex);
	platform->SetWindowTitle(ScratchPrintStr("%.*s - %s", StrPrint(app->currentTab->filePath), PROJECT_READABLE_NAME_STR));
}

void AddTooltipForFileOption(FileOption* option, uxx lineIndex)
{
	ScratchBegin(scratch);
	Str8 tooltipStr = PrintInArenaStr(scratch, "%llu: %.*s", lineIndex, StrPrint(option->name));
	Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(option->name));
	ClayId btnId = ToClayId(btnIdStr);
	TooltipRegion* tooltip = AddTooltipClay(&app->tooltipRegions, btnId, tooltipStr, OPTION_NAME_TOOLTIP_DELAY, 0);
	tooltip->clayContainerId = CLAY_ID("OptionsList");
	option->tooltipId = tooltip->id;
	ScratchEnd(scratch);
}

Str8 GetOptionNameAbbreviation(Arena* arena, Str8 fullName)
{
	Str8 result = Str8_Empty;
	for (uxx pass = 0; pass < 2; pass++)
	{
		uxx numChars = 0;
		bool prevCharWasCap = false;
		bool prevCharWasAlpha = false;
		for (uxx bIndex = 0; bIndex < fullName.length; bIndex++)
		{
			char nextChar = fullName.chars[bIndex];
			bool isAlpha = IsCharAlphaNumeric(CharToU32(nextChar));
			bool isCapitalized = IsCharUppercaseAlphabet(CharToU32(nextChar)) || IsCharNumeric(CharToU32(nextChar));
			if (numChars == 0)
			{
				if (result.chars != nullptr) { result.chars[numChars] = nextChar; }
				numChars++;
			}
			else if (!prevCharWasAlpha && isAlpha)
			{
				if (result.chars != nullptr) { result.chars[numChars] = nextChar; }
				numChars++;
			}
			else if (prevCharWasAlpha && isAlpha && !prevCharWasCap && isCapitalized)
			{
				if (result.chars != nullptr) { result.chars[numChars] = nextChar; }
				numChars++;
			}
			prevCharWasAlpha = isAlpha;
			prevCharWasCap = isCapitalized;
		}
		
		if (pass == 0)
		{
			if (numChars == 0) { return Str8_Empty; }
			result.length = numChars;
			result.chars = (char*)AllocMem(arena, result.length);
			NotNull(result.chars);
		}
		else { Assert(numChars == result.length); }
	}
	return result;
}

void CalculateLongestAbbreviationWidth(FileTab* tab)
{
	NotNull(tab);
	tab->longestAbbreviationWidth = 0;
	VarArrayLoop(&tab->fileOptions, oIndex)
	{
		VarArrayLoopGet(FileOption, option, &tab->fileOptions, oIndex);
		v2 abbreviationSize = ClayUiTextSize(&app->mainFont, app->mainFontSize, MAIN_FONT_STYLE, option->abbreviation);
		if (tab->longestAbbreviationWidth < abbreviationSize.Width) { tab->longestAbbreviationWidth = abbreviationSize.Width; }
	}
	tab->longestAbbreviationWidthFontSize = app->mainFontSize;
}

void UpdateFileTabOptions(FileTab* tab)
{
	ScratchBegin(scratch);
	
	VarArrayLoop(&tab->fileOptions, oIndex)
	{
		VarArrayLoopGet(FileOption, option, &tab->fileOptions, oIndex);
		FreeFileOption(option);
	}
	VarArrayClear(&tab->fileOptions);
	
	Str8 commentStartStr = StrLit("//");
	LineParser lineParser = MakeLineParser(tab->fileContents);
	Str8 fullLine = Str8_Empty;
	FileOption* prevOption = nullptr;
	while (LineParserGetLine(&lineParser, &fullLine))
	{
		uxx scratchMark = ArenaGetMark(scratch);
		Str8 line = TrimWhitespace(fullLine);
		Str8 lineComment = Str8_Empty;
		uxx commentSlashesIndex = StrExactFind(line, commentStartStr);
		if (commentSlashesIndex < line.length)
		{
			lineComment = StrSliceFrom(line, commentSlashesIndex);
			line = StrSlice(line, 0, commentSlashesIndex);
			line = TrimWhitespace(line);
		}
		
		bool isOption = false;
		Str8 defineStr = StrLit("#define");
		if (IsEmptyStr(line) && !IsEmptyStr(lineComment))
		{
			uxx commentStartIndex = lineParser.lineBeginByteIndex + (uxx)(lineComment.chars - fullLine.chars);
			Str8 commentContents = TrimWhitespace(StrSliceFrom(lineComment, commentStartStr.length));
			if (StrExactStartsWith(commentContents, defineStr))
			{
				uxx defineStartIndex = lineParser.lineBeginByteIndex + (uxx)(commentContents.chars - fullLine.chars);
				Str8 namePart = TrimWhitespace(StrSliceFrom(commentContents, defineStr.length));
				if (!IsEmptyStr(namePart))
				{
					FileOption* newOption = VarArrayAdd(FileOption, &tab->fileOptions);
					NotNull(newOption);
					ClearPointer(newOption);
					newOption->name = AllocStr8(stdHeap, namePart);
					newOption->abbreviation = GetOptionNameAbbreviation(stdHeap, newOption->name);
					newOption->type = FileOptionType_CommentDefine;
					newOption->isUncommented = false;
					newOption->fileContentsStartIndex = commentStartIndex;
					newOption->fileContentsEndIndex = defineStartIndex;
					newOption->valueStr = AllocStr8(stdHeap, commentStartStr);
					AddTooltipForFileOption(newOption, lineParser.lineIndex);
					prevOption = newOption;
					isOption = true;
				}
			}
		}
		else if (StrExactStartsWith(line, defineStr))
		{
			uxx lineStartIndex = lineParser.lineBeginByteIndex + (uxx)(line.chars - fullLine.chars);
			uxx lineEndIndex = lineStartIndex + line.length;
			const char* possibleBoolValues[] = { "1", "0", "true", "false" }; //NOTE: These must alternate truthy/falsey so %2 logic below works
			for (uxx vIndex = 0; vIndex < ArrayCount(possibleBoolValues); vIndex++)
			{
				Str8 boolValueStr = MakeStr8Nt(possibleBoolValues[vIndex]);
				if (line.length >= defineStr.length + 1 + boolValueStr.length &&
					StrExactEndsWith(line, boolValueStr) &&
					IsCharWhitespace(line.chars[line.length-boolValueStr.length-1], false))
				{
					FileOption* newOption = VarArrayAdd(FileOption, &tab->fileOptions);
					NotNull(newOption);
					ClearPointer(newOption);
					newOption->name = TrimWhitespace(StrSlice(line, defineStr.length, line.length - boolValueStr.length));
					newOption->name = AllocStr8(stdHeap, newOption->name);
					newOption->abbreviation = GetOptionNameAbbreviation(stdHeap, newOption->name);
					newOption->type = FileOptionType_Bool;
					newOption->valueBool = ((vIndex%2) == 0);
					newOption->fileContentsStartIndex = lineEndIndex - boolValueStr.length;
					newOption->fileContentsEndIndex = lineEndIndex;
					newOption->valueStr = AllocStr8(stdHeap, StrSlice(tab->fileContents, newOption->fileContentsStartIndex, newOption->fileContentsEndIndex));
					AddTooltipForFileOption(newOption, lineParser.lineIndex);
					prevOption = newOption;
					isOption = true;
					break;
				}
			}
			
			if (!isOption)
			{
				// If we trim the part after the #define and we find
				// that the leftover part is a valid C identifier then
				// we can treat this #define as something that can be
				// commented out because it has no value
				Str8 namePart = TrimWhitespace(StrSliceFrom(line, defineStr.length));
				if (IsValidIdentifier(namePart.length, namePart.chars, false, false, false))
				{
					FileOption* newOption = VarArrayAdd(FileOption, &tab->fileOptions);
					NotNull(newOption);
					ClearPointer(newOption);
					newOption->name = AllocStr8(stdHeap, namePart);
					newOption->abbreviation = GetOptionNameAbbreviation(stdHeap, newOption->name);
					newOption->type = FileOptionType_CommentDefine;
					newOption->isUncommented = true;
					newOption->fileContentsStartIndex = lineStartIndex;
					newOption->fileContentsEndIndex = lineStartIndex;
					newOption->valueStr = Str8_Empty;
					AddTooltipForFileOption(newOption, lineParser.lineIndex);
					prevOption = newOption;
					isOption = true;
				}
			}
		}
		else if (StrExactContains(line, StrLit("::"))) //jai constant syntax
		{
			uxx lineStartIndex = lineParser.lineBeginByteIndex + (uxx)(line.chars - fullLine.chars);
			uxx colonsIndex = StrExactFind(line, StrLit("::"));
			Str8 namePart = TrimWhitespace(StrSlice(line, 0, colonsIndex));
			Str8 valuePart = TrimWhitespace(StrSliceFrom(line, colonsIndex+2));
			if (IsValidIdentifier(namePart.length, namePart.chars, false, false, false) &&
				StrExactEndsWith(valuePart, StrLit(";")))
			{
				valuePart = StrSlice(valuePart, 0, valuePart.length-1);
				
				bool isBooleanValue = false;
				bool isBooleanTrue = false;
				const char* possibleBoolValues[] = { "1", "0", "true", "false" }; //NOTE: These must alternate truthy/falsey so %2 logic below works
				for (uxx vIndex = 0; vIndex < ArrayCount(possibleBoolValues); vIndex++)
				{
					Str8 boolValueStr = MakeStr8Nt(possibleBoolValues[vIndex]);
					if (StrExactEquals(valuePart, boolValueStr)) { isBooleanValue = true; isBooleanTrue = ((vIndex%2) == 0); break; }
				}
				
				if (isBooleanValue)
				{
					FileOption* newOption = VarArrayAdd(FileOption, &tab->fileOptions);
					NotNull(newOption);
					ClearPointer(newOption);
					newOption->name = AllocStr8(stdHeap, namePart);
					newOption->abbreviation = GetOptionNameAbbreviation(stdHeap, newOption->name);
					newOption->type = FileOptionType_Bool;
					newOption->valueBool = isBooleanTrue;
					newOption->fileContentsStartIndex = lineStartIndex + (uxx)(valuePart.chars - line.chars);
					newOption->fileContentsEndIndex = newOption->fileContentsStartIndex + valuePart.length;
					newOption->valueStr = AllocStr8(stdHeap, StrSlice(tab->fileContents, newOption->fileContentsStartIndex, newOption->fileContentsEndIndex));
					AddTooltipForFileOption(newOption, lineParser.lineIndex);
					prevOption = newOption;
					isOption = true;
				}
			}
		}
		
		if (!isOption && IsEmptyStr(line) && IsEmptyStr(lineComment) && prevOption != nullptr && prevOption->numEmptyLinesAfter < MAX_LINE_BREAKS_CONSIDERED)
		{
			IncrementU64(prevOption->numEmptyLinesAfter);
		}
		ArenaResetToMark(scratch, scratchMark);
	}
	
	CalculateLongestAbbreviationWidth(tab);
	
	ScratchEnd(scratch);
}

//NOTE: This function automatically sets the tab as the currentTab (it will also focus an existing tab if the file is already open)
FileTab* AppOpenFileTab(FilePath filePath)
{
	FileTab* existingTab = AppFindTabForPath(filePath);
	if (existingTab != nullptr)
	{
		uxx existingTabIndex = 0;
		bool foundTabIndex = VarArrayGetIndexOf(FileTab, &app->tabs, existingTab, &existingTabIndex);
		Assert(foundTabIndex);
		AppChangeTab(existingTabIndex);
		return existingTab;
	}
	
	Str8 fileContents = Str8_Empty;
	bool openResult = OsReadTextFile(filePath, stdHeap, &fileContents);
	if (!openResult) { NotifyPrint_W("Failed to open file at \"%.*s\"", StrPrint(filePath)); return nullptr; }
	
	FileTab* newTab = VarArrayAdd(FileTab, &app->tabs);
	NotNull(newTab);
	ClearPointer(newTab);
	//TODO: Should we maybe check if the file is already open in an existing tab?
	// if (app->isFileOpen) { AppCloseFile(); }
	newTab->fileContents = fileContents;
	newTab->originalFileContents = AllocStr8(stdHeap, newTab->fileContents);
	newTab->isFileChangedFromOriginal = false;
	newTab->filePath = AllocStr8(stdHeap, filePath);
	InitVarArray(FileOption, &newTab->fileOptions, stdHeap);
	
	UpdateFileTabOptions(newTab);
	
	newTab->fileWatchId = AddFileWatch(&app->fileWatches, newTab->filePath, CHECK_FILE_WRITE_TIME_PERIOD);
	
	AppChangeTab(app->tabs.length-1);
	
	AppRememberRecentFile(filePath);
	
	return newTab;
}

void AppReloadFileTab(uxx tabIndex)
{
	Assert(tabIndex < app->tabs.length);
	FileTab* tab = VarArrayGetHard(FileTab, &app->tabs, tabIndex);
	Str8 fileContents = Str8_Empty;
	bool openResult = OsReadTextFile(tab->filePath, stdHeap, &fileContents);
	if (!openResult)
	{
		NotifyPrint_W("Failed to reload file at \"%.*s\"", StrPrint(tab->filePath));
		AppCloseFileTab(tabIndex);
		return;
	}
	
	FreeStr8(stdHeap, &tab->fileContents);
	FreeStr8(stdHeap, &tab->originalFileContents);
	tab->fileContents = fileContents;
	tab->originalFileContents = AllocStr8(stdHeap, tab->fileContents);
	tab->isFileChangedFromOriginal = false;
	
	UpdateFileTabOptions(tab);
}

bool AppCheckForFileChanges()
{
	bool didAnyFileChange = false;
	VarArrayLoop(&app->tabs, tIndex)
	{
		VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
		if (HasFileWatchChangedWithDelay(&app->fileWatches, tab->fileWatchId, FILE_RELOAD_DELAY))
		{
			ClearFileWatchChanged(&app->fileWatches, tab->fileWatchId);
			PrintLine_N("File[%llu] changed externally! Reloading...", (u64)tIndex);
			AppReloadFileTab(tIndex);
			didAnyFileChange = true;
			break; //NOTE: We only reload a single file a frame, because reload may fail and remove the tab from the array, so continuing iteration is dangerous
		}
	}
	return didAnyFileChange;
}

//TODO: If we see that the file write time has changed right before we go to change the file, maybe we should reload the file instead? Let external edits take precident?
void UpdateOptionValueInFile(FileTab* tab, FileOption* option)
{
	ScratchBegin(scratch);
	NotNull(tab);
	NotNull(option);
	NotNullStr(option->valueStr);
	DebugAssert(VarArrayContains(FileOption, &tab->fileOptions, option));
	if (!StrExactEquals(option->valueStr, StrSlice(tab->fileContents, option->fileContentsStartIndex, option->fileContentsEndIndex)))
	{
		Str8 fileBeginning = StrSlice(tab->fileContents, 0, option->fileContentsStartIndex);
		Str8 fileEnd = StrSliceFrom(tab->fileContents, option->fileContentsEndIndex);
		Str8 newFileContents = JoinStringsInArena(scratch, fileBeginning, option->valueStr, false);
		newFileContents = JoinStringsInArena(scratch, newFileContents, fileEnd, false);
		bool writeResult = OsWriteTextFile(tab->filePath, newFileContents);
		if (writeResult)
		{
			if (newFileContents.length != tab->fileContents.length)
			{
				i64 byteOffset = (i64)newFileContents.length - (i64)tab->fileContents.length;
				VarArrayLoop(&tab->fileOptions, oIndex)
				{
					VarArrayLoopGet(FileOption, otherOption, &tab->fileOptions, oIndex);
					if (otherOption != option && otherOption->fileContentsStartIndex >= option->fileContentsEndIndex)
					{
						otherOption->fileContentsStartIndex += byteOffset;
						otherOption->fileContentsEndIndex += byteOffset;
					}
				}
			}
			option->fileContentsEndIndex = option->fileContentsStartIndex + option->valueStr.length;
			
			FreeStr8(stdHeap, &tab->fileContents);
			tab->fileContents = AllocStr8(stdHeap, newFileContents);
			tab->isFileChangedFromOriginal = !StrExactEquals(tab->fileContents, tab->originalFileContents);
			
			//Since we just wrote to the file, make sure we immediately updated out file write time so we don't think it was an external change
			ClearFileWatchChanged(&app->fileWatches, tab->fileWatchId);
		}
		else
		{
			NotifyPrint_E("Failed to write %llu byte%s to file \"%.*s\"!", (u64)newFileContents.length, Plural(newFileContents.length, "s"), StrPrint(tab->filePath));
			FreeStr8(stdHeap, &option->valueStr);
			option->valueStr = AllocStr8(stdHeap, StrSlice(tab->fileContents, option->fileContentsStartIndex, option->fileContentsEndIndex));
		}
	}
	ScratchEnd(scratch);
}

void SetOptionValue(FileTab* tab, FileOption* option, Str8 newValueStr)
{
	NotNull(tab);
	NotNull(option);
	if (!StrExactEquals(option->valueStr, newValueStr))
	{
		FreeStr8(stdHeap, &option->valueStr);
		option->valueStr = AllocStr8(stdHeap, newValueStr);
		UpdateOptionValueInFile(tab, option);
	}
}

// +==================================+
// | AppResetCurrentFilePopupCallback |
// +==================================+
// void AppResetCurrentFilePopupCallback(PopupDialogResult result, PopupDialog* dialog, PopupDialogButton* selectedButton, void* contextPntr)
POPUP_DIALOG_CALLBACK_DEF(AppResetCurrentFilePopupCallback)
{
	UNUSED(dialog); UNUSED(selectedButton); UNUSED(contextPntr);
	if (result == PopupDialogResult_Yes && app->currentTab != nullptr)
	{
		bool writeSuccess = OsWriteTextFile(app->currentTab->filePath, app->currentTab->originalFileContents);
		if (!writeSuccess) { NotifyPrint_E("Failed to write to file at \"%.*s\"!", StrPrint(app->currentTab->filePath)); }
		else
		{
			AppReloadFileTab(app->currentTabIndex);
		}
	}
}
