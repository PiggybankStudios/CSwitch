/*
File:   app_helpers.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** None
*/

ImageData LoadImageData(Arena* arena, const char* path)
{
	ScratchBegin1(scratch, arena);
	Slice fileContents = Slice_Empty;
	// bool readFileResult = OsReadFile(FilePathLit(path), scratch, false, &fileContents);
	// Assert(readFileResult);
	Result readFileResult = TryReadAppResource(&app->resources, scratch, FilePathLit(path), false, &fileContents);
	Assert(readFileResult == Result_Success);
	ImageData imageData = ZEROED;
	Result parseResult = TryParseImageFile(fileContents, arena, &imageData);
	Assert(parseResult == Result_Success);
	ScratchEnd(scratch);
	return imageData;
}

void LoadWindowIcon()
{
	ScratchBegin(scratch);
	ImageData iconImageDatas[6];
	iconImageDatas[0] = LoadImageData(scratch, "resources/image/icon_16.png");
	iconImageDatas[1] = LoadImageData(scratch, "resources/image/icon_24.png");
	iconImageDatas[2] = LoadImageData(scratch, "resources/image/icon_32.png");
	iconImageDatas[3] = LoadImageData(scratch, "resources/image/icon_64.png");
	iconImageDatas[4] = LoadImageData(scratch, "resources/image/icon_120.png");
	iconImageDatas[5] = LoadImageData(scratch, "resources/image/icon_256.png");
	platform->SetWindowIcon(ArrayCount(iconImageDatas), &iconImageDatas[0]);
	ScratchEnd(scratch);
}

bool AppCreateFonts()
{
	FontCharRange fontCharRanges[] = {
		FontCharRange_ASCII,
		FontCharRange_LatinExt,
		NewFontCharRangeSingle(UNICODE_ELLIPSIS_CODEPOINT),
		NewFontCharRangeSingle(UNICODE_RIGHT_ARROW_CODEPOINT),
	};
	
	// ImageData keysSheet = LoadImageData(scratch, "resources/image/keys16.png");
	// ImageData keysWideSheet = LoadImageData(scratch, "resources/image/keys16_wide.png");
	PigFont newUiFont = ZEROED;
	{
		#if 0
		uxx numKeyCodepoints = 0;
		CustomFontGlyph customGlyphs[KEY_CODEPOINT_COUNT];
		for (uxx keyIndex = 0; keyIndex < KEY_CODEPOINT_COUNT; keyIndex++)
		{
			u32 codepoint = (u32)(KEY_FIRST_CODEPOINT + keyIndex);
			i32 keyGlyphWidth = 0;
			bool useWideSheet = false;
			v2i tilePos = GetSheetFrameForKey(GetKeyForCodepoint(codepoint), false, &keyGlyphWidth, &useWideSheet);
			v2i sheetTileSize = useWideSheet ? NewV2i(32, 16) : FillV2i(16);
			i32 glyphOffset = (i32)(sheetTileSize.Width - keyGlyphWidth) / 2;
			customGlyphs[numKeyCodepoints].codepoint = codepoint;
			customGlyphs[numKeyCodepoints].imageData = useWideSheet ? keysWideSheet : keysSheet;
			customGlyphs[numKeyCodepoints].sourceRec = NewReci(tilePos.X * sheetTileSize.Width + glyphOffset, tilePos.Y * sheetTileSize.Height, keyGlyphWidth, sheetTileSize.Height);
			numKeyCodepoints++;
		}
		CustomFontCharRange customGlyphsRange = NewCustomFontCharRange(numKeyCodepoints, &customGlyphs[0]);
		#endif
		newUiFont = InitFont(stdHeap, StrLit("uiFont"));
		Result attachResult = AttachOsTtfFileToFont(&newUiFont, StrLit(UI_FONT_NAME), app->uiFontSize, UI_FONT_STYLE);
		Assert(attachResult == Result_Success);
		Result bakeResult = BakeFontAtlas(&newUiFont, app->uiFontSize, UI_FONT_STYLE, NewV2i(256, 256), ArrayCount(fontCharRanges), &fontCharRanges[0]);
		if (bakeResult != Result_Success)
		{
			bakeResult = BakeFontAtlas(&newUiFont, app->uiFontSize, UI_FONT_STYLE, NewV2i(512, 512), ArrayCount(fontCharRanges), &fontCharRanges[0]);
			if (bakeResult != Result_Success)
			{
				RemoveAttachedTtfFile(&newUiFont);
				FreeFont(&newUiFont);
				return false;
			}
		}
		Assert(bakeResult == Result_Success);
		FillFontKerningTable(&newUiFont);
		RemoveAttachedTtfFile(&newUiFont);
	}
	
	PigFont newMainFont = ZEROED;
	{
		newMainFont = InitFont(stdHeap, StrLit("mainFont"));
		Result attachResult = AttachOsTtfFileToFont(&newMainFont, StrLit(MAIN_FONT_NAME), app->mainFontSize, MAIN_FONT_STYLE);
		Assert(attachResult == Result_Success);
		
		Result bakeResult = BakeFontAtlas(&newMainFont, app->mainFontSize, MAIN_FONT_STYLE, NewV2i(256, 256), ArrayCount(fontCharRanges), &fontCharRanges[0]);
		if (bakeResult != Result_Success)
		{
			bakeResult = BakeFontAtlas(&newMainFont, app->mainFontSize, MAIN_FONT_STYLE, NewV2i(512, 512), ArrayCount(fontCharRanges), &fontCharRanges[0]);
			if (bakeResult != Result_Success)
			{
				RemoveAttachedTtfFile(&newMainFont);
				FreeFont(&newMainFont);
				FreeFont(&newUiFont);
				return false;
			}
		}
		FillFontKerningTable(&newMainFont);
		RemoveAttachedTtfFile(&newMainFont);
	}
	
	if (app->uiFont.arena != nullptr) { FreeFont(&app->uiFont); }
	if (app->mainFont.arena != nullptr) { FreeFont(&app->mainFont); }
	app->uiFont = newUiFont;
	app->mainFont = newMainFont;
	return true;
}

bool AppChangeFontSize(bool increase)
{
	if (increase)
	{
		app->uiFontSize += 1;
		app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
		app->uiScale = app->uiFontSize / (r32)DEFAULT_UI_FONT_SIZE;
		if (!AppCreateFonts())
		{
			app->uiFontSize -= 1;
			app->uiScale = app->uiFontSize / (r32)DEFAULT_UI_FONT_SIZE;
			app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
		}
		return true;
	}
	else if (AreSimilarOrGreaterR32(app->uiFontSize - 1.0f, MIN_UI_FONT_SIZE, DEFAULT_R32_TOLERANCE))
	{
		app->uiFontSize -= 1;
		app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
		app->uiScale = app->uiFontSize / (r32)DEFAULT_UI_FONT_SIZE;
		AppCreateFonts();
		return true;
	}
	else { return false; }
}

void AppClearRecentFiles()
{
	VarArrayLoop(&app->recentFiles, rIndex)
	{
		VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex);
		FreeStr8(stdHeap, &recentFile->path);
	}
	VarArrayClear(&app->recentFiles);
}

void AppLoadRecentFilesList()
{
	ScratchBegin(scratch);
	FilePath settingsFolderPath = OsGetSettingsSavePath(scratch, Str8_Empty, StrLit(PROJECT_FOLDER_NAME_STR), false);
	FilePath savePath = PrintInArenaStr(scratch, "%.*s%s%s",
		StrPrint(settingsFolderPath),
		DoesPathHaveTrailingSlash(settingsFolderPath) ? "" : "/",
		RECENT_FILES_SAVE_FILEPATH
	);
	
	if (OsDoesFileExist(savePath))
	{
		Str8 fileContent = Str8_Empty;
		if (OsReadTextFile(savePath, scratch, &fileContent))
		{
			AppClearRecentFiles();
			LineParser parser = NewLineParser(fileContent);
			Str8 fileLine = ZEROED;
			while (LineParserGetLine(&parser, &fileLine))
			{
				if (!IsEmptyStr(fileLine))
				{
					RecentFile* newFile = VarArrayAdd(RecentFile, &app->recentFiles);
					NotNull(newFile);
					newFile->path = AllocStr8(stdHeap, fileLine);
					newFile->fileExists = OsDoesFileExist(newFile->path);
				}
			}
			
			PrintLine_D("Loaded %llu recent file%s from \"%.*s\"", app->recentFiles.length, Plural(app->recentFiles.length, "s"), StrPrint(savePath));
			if (app->recentFilesWatchId == 0) { app->recentFilesWatchId = AddFileWatch(&app->fileWatches, savePath, CHECK_RECENT_FILES_WRITE_TIME_PERIOD); }
			else { ClearFileWatchChanged(&app->fileWatches, app->recentFilesWatchId); }
		}
		else { PrintLine_W("No recent files save found at \"%.*s\"", StrPrint(savePath)); }
	}
	ScratchEnd(scratch);
}

void AppSaveRecentFilesList()
{
	ScratchBegin(scratch);
	FilePath settingsFolderPath = OsGetSettingsSavePath(scratch, Str8_Empty, StrLit(PROJECT_FOLDER_NAME_STR), true);
	FilePath savePath = PrintInArenaStr(scratch, "%.*s%s%s",
		StrPrint(settingsFolderPath),
		DoesPathHaveTrailingSlash(settingsFolderPath) ? "" : "/",
		RECENT_FILES_SAVE_FILEPATH
	);
	
	OsFile fileHandle = ZEROED;
	if (OsOpenFile(scratch, savePath, OsOpenFileMode_Write, false, &fileHandle))
	{
		VarArrayLoop(&app->recentFiles, rIndex)
		{
			VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex);
			bool writeSuccess = OsWriteToOpenTextFile(&fileHandle, recentFile->path);
			Assert(writeSuccess);
			writeSuccess = OsWriteToOpenTextFile(&fileHandle, StrLit("\n"));
			Assert(writeSuccess);
		}
		OsCloseFile(&fileHandle);
		
		if (app->recentFilesWatchId == 0)
		{
			app->recentFilesWatchId = AddFileWatch(&app->fileWatches, savePath, CHECK_RECENT_FILES_WRITE_TIME_PERIOD);
		}
		else
		{
			ClearFileWatchChanged(&app->fileWatches, app->recentFilesWatchId);
		}
	}
	else { PrintLine_E("Failed to save recent files list to \"%.*s\"", StrPrint(savePath)); }
	
	ScratchEnd(scratch);
}

void AppRememberRecentFile(FilePath filePath)
{
	ScratchBegin(scratch);
	Str8 fullPath = OsGetFullPath(scratch, filePath);
	bool alreadyExists = false;
	VarArrayLoop(&app->recentFiles, rIndex)
	{
		VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex);
		if (StrAnyCaseEquals(recentFile->path, fullPath))
		{
			alreadyExists = true;
			if (rIndex+1 < app->recentFiles.length)
			{
				//Move this path to the end of the array
				RecentFile temp;
				MyMemCopy(&temp, recentFile, sizeof(RecentFile));
				VarArrayRemove(RecentFile, &app->recentFiles, recentFile);
				RecentFile* newRecentFile = VarArrayAdd(RecentFile, &app->recentFiles);
				MyMemCopy(newRecentFile, &temp, sizeof(RecentFile));
				break;
			}
		}
	}
	
	if (!alreadyExists)
	{
		RecentFile* newRecentFile = VarArrayAdd(RecentFile, &app->recentFiles);
		NotNull(newRecentFile);
		ClearPointer(newRecentFile);
		newRecentFile->path = AllocStr8(stdHeap, fullPath);
		newRecentFile->fileExists = true;
		NotNull(newRecentFile->path.chars);
		
		while (app->recentFiles.length > RECENT_FILES_MAX_LENGTH)
		{
			RecentFile* firstFile = VarArrayGetFirst(RecentFile, &app->recentFiles);
			FreeStr8(stdHeap, &firstFile->path);
			VarArrayRemoveFirst(RecentFile, &app->recentFiles);
		}
	}
	
	AppSaveRecentFilesList();
	
	ScratchEnd(scratch);
}

Str8 GetUniqueRecentFilePath(FilePath filePath)
{
	if (IsEmptyStr(filePath)) { return filePath; }
	uxx numParts = CountPathParts(filePath, false);
	if (numParts == 1) { return filePath; }
	
	for (uxx pIndex = 0; pIndex < numParts; pIndex++)
	{
		// Str8 GetPathPart(FilePath path, ixx partIndex, bool includeEmptyBeginOrEnd)
		Str8 pathPart = GetPathPart(filePath, numParts-1 - pIndex, false);
		uxx partStartIndex = (uxx)(pathPart.chars - filePath.chars);
		Str8 subPath = StrSliceFrom(filePath, partStartIndex);
		
		bool foundConflict = false;
		VarArrayLoop(&app->recentFiles, rIndex)
		{
			VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex);
			if (!StrAnyCaseEquals(recentFile->path, filePath) &&
				StrAnyCaseEndsWith(recentFile->path, subPath))
			{
				foundConflict = true;
				break;
			}
		}
		if (!foundConflict) { return subPath; }
	}
	
	return filePath;
}

Str8 GetUniqueTabFilePath(FilePath filePath)
{
	if (IsEmptyStr(filePath)) { return filePath; }
	uxx numParts = CountPathParts(filePath, false);
	if (numParts == 1) { return filePath; }
	
	for (uxx pIndex = 0; pIndex < numParts; pIndex++)
	{
		// Str8 GetPathPart(FilePath path, ixx partIndex, bool includeEmptyBeginOrEnd)
		Str8 pathPart = GetPathPart(filePath, numParts-1 - pIndex, false);
		uxx partStartIndex = (uxx)(pathPart.chars - filePath.chars);
		Str8 subPath = StrSliceFrom(filePath, partStartIndex);
		
		bool foundConflict = false;
		VarArrayLoop(&app->tabs, tIndex)
		{
			VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
			if (!StrAnyCaseEquals(tab->filePath, filePath) &&
				StrAnyCaseEndsWith(tab->filePath, subPath))
			{
				foundConflict = true;
				break;
			}
		}
		if (!foundConflict) { return subPath; }
	}
	
	return filePath;
}

// +==================================+
// | AppClearRecentFilesPopupCallback |
// +==================================+
// void AppClearRecentFilesPopupCallback(PopupDialogResult result, PopupDialog* dialog, PopupDialogButton* selectedButton, void* contextPntr)
POPUP_DIALOG_CALLBACK_DEF(AppClearRecentFilesPopupCallback)
{
	UNUSED(dialog); UNUSED(selectedButton); UNUSED(contextPntr);
	if (result == PopupDialogResult_Yes)
	{
		AppClearRecentFiles();
		AppSaveRecentFilesList();
	}
}
