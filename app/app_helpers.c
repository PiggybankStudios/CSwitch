/*
File:   app_helpers.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** None
*/

FilePath GetAppSettingsSavePath(Arena* arena, bool addNullTerm)
{
	ScratchBegin1(scratch, arena);
	FilePath folderPath = OsGetSettingsSavePath(scratch, Str8_Empty, StrLit(PROJECT_FOLDER_NAME_STR), true);
	Assert(!IsEmptyStr(folderPath));
	FilePath result = JoinStringsInArenaWithChar(arena, folderPath, '/', StrLit(SETTINGS_FILENAME), addNullTerm);
	ScratchEnd(scratch);
	return result;
}
void SaveAppSettings()
{
	ScratchBegin(scratch);
	FilePath settingsFilePath = GetAppSettingsSavePath(scratch, false);
	WriteLine_D("Saving settings...");
	bool saveSuccess = TrySaveAppSettingsTo(&app->settings, settingsFilePath);
	if (!saveSuccess)
	{
		DebugAssert(saveSuccess);
		NotifyPrint_E("Failed to save settings file! Make sure the folder has write permissions for the current user!\nPath: \"%.*s\"", StrPrint(settingsFilePath));
	}
	ScratchEnd(scratch);
}
void LoadAppSettings()
{
	ScratchBegin(scratch);
	FilePath settingsFilePath = GetAppSettingsSavePath(scratch, false);
	if (OsDoesFileExist(settingsFilePath))
	{
		Result loadSettingsResult = TryLoadAppSettingsFrom(settingsFilePath, &app->settings);
		if (loadSettingsResult == Result_Success)
		{
			PrintLine_I("Loaded settings from \"%.*s\"", StrPrint(settingsFilePath));
		}
		else
		{
			NotifyPrint_W("Failed to load settings!\nError: %s\nPath: \"%.*s\"", GetResultStr(loadSettingsResult), StrPrint(settingsFilePath));
		}
	}
	else
	{
		PrintLine_N("No settings file found at \"%.*s\"! Saving settings", StrPrint(settingsFilePath));
		SaveAppSettings();
	}
	ScratchEnd(scratch);
}

ImageData LoadImageData(Arena* arena, const char* path)
{
	ScratchBegin1(scratch, arena);
	Slice fileContents = Slice_Empty;
	// bool readFileResult = OsReadFile(FilePathLit(path), scratch, false, &fileContents);
	// Assert(readFileResult);
	Result readFileResult = TryReadAppResource(&app->resources, scratch, MakeFilePathNt(path), false, &fileContents);
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

void LoadNotificationIcons()
{
	{
		ScratchBegin(scratch);
		ImageData imageData = LoadImageData(scratch, NOTIFICATION_ICONS_TEXTURE_PATH);
		AssertMsg(imageData.pixels != nullptr && imageData.size.Width > 0 && imageData.size.Height > 0, "Failed to load notification icons texture!");
		Texture newTexture = InitTexture(stdHeap, StrLit("notificationIcons"), imageData.size, imageData.pixels, 0x00);
		AssertMsg(newTexture.error == Result_Success, "Failed to init texture for notification icons!");
		FreeTexture(&app->notificationIconsTexture);
		app->notificationIconsTexture = newTexture;
		ScratchEnd(scratch);
	}
	
	const v2i sheetSize = { .X=2, .Y=2 };
	v2 cellSize = MakeV2(
		(r32)app->notificationIconsTexture.Width / (r32)sheetSize.Width,
		(r32)app->notificationIconsTexture.Height / (r32)sheetSize.Height
	);
	r32 iconScale = NOTIFICATION_ICONS_SIZE / cellSize.Width;
	for (uxx lIndex = DbgLevel_Debug; lIndex < DbgLevel_Count; lIndex++)
	{
		DbgLevel dbgLevel = (DbgLevel)lIndex;
		v2i cellPos = V2i_Zero;
		switch (dbgLevel)
		{
			// case DbgLevel_Debug:   cellPos = MakeV2i(1, 0); break;
			case DbgLevel_Regular: cellPos = MakeV2i(1, 0); break;
			case DbgLevel_Info:    cellPos = MakeV2i(1, 0); break;
			case DbgLevel_Notify:  cellPos = MakeV2i(1, 0); break;
			case DbgLevel_Other:   cellPos = MakeV2i(1, 0); break;
			case DbgLevel_Warning: cellPos = MakeV2i(0, 1); break;
			case DbgLevel_Error:   cellPos = MakeV2i(1, 1); break;
		}
		if (cellPos.X != 0 || cellPos.Y != 0)
		{
			rec iconSourceRec = MakeRec(
				cellSize.Width * cellPos.X,
				cellSize.Height * cellPos.Y,
				cellSize.Width, cellSize.Height
			);
			SetNotificationIconEx(&app->notificationQueue, dbgLevel, &app->notificationIconsTexture, iconScale, GetDbgLevelTextColor(dbgLevel), iconSourceRec);
		}
	}
}

Slice TryLoadFontFileFromResources(Arena* arena, Str8 filePath)
{
	Slice fileContents = Slice_Empty;
	Result result = TryReadAppResource(&app->resources, arena, filePath, false, &fileContents);
	Assert(result == Result_Success);
	return fileContents;
}
Result TryAttachFontFileFromResources(PigFont* font, Str8 filePath, u8 fontStyle)
{
	ScratchBegin1(scratch, font->arena);
	Slice fontFileContents = TryLoadFontFileFromResources(scratch, filePath);
	Assert(fontFileContents.length > 0);
	Result attachResult = TryAttachFontFile(font, filePath, fontFileContents, fontStyle, true);
	ScratchEnd(scratch);
	return attachResult;
}

bool AppCreateFonts()
{
	FontCharRange fontCharRanges[] = {
		FontCharRange_ASCII,
		// FontCharRange_LatinExtA,
		MakeFontCharRangeSingle(UNICODE_ELLIPSIS_CODEPOINT),
		MakeFontCharRangeSingle(UNICODE_RIGHT_ARROW_CODEPOINT),
	};
	
	Result attachResult = Result_None;
	Result bakeResult = Result_None;
	
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
			v2i sheetTileSize = useWideSheet ? MakeV2i(32, 16) : FillV2i(16);
			i32 glyphOffset = (i32)(sheetTileSize.Width - keyGlyphWidth) / 2;
			customGlyphs[numKeyCodepoints].codepoint = codepoint;
			customGlyphs[numKeyCodepoints].imageData = useWideSheet ? keysWideSheet : keysSheet;
			customGlyphs[numKeyCodepoints].sourceRec = MakeReci(tilePos.X * sheetTileSize.Width + glyphOffset, tilePos.Y * sheetTileSize.Height, keyGlyphWidth, sheetTileSize.Height);
			numKeyCodepoints++;
		}
		CustomFontCharRange customGlyphsRange = MakeCustomFontCharRangeArray(numKeyCodepoints, &customGlyphs[0]);
		#endif
		
		newUiFont = InitFont(stdHeap, StrLit("uiFont"));
		#if defined(UI_FONT_PATH)
		attachResult = TryAttachFontFileFromResources(&newUiFont, StrLit(UI_FONT_PATH), UI_FONT_STYLE);
		Assert(attachResult == Result_Success);
		#else //defined(UI_FONT_NAME)
		attachResult = TryAttachOsTtfFileToFont(&newUiFont, StrLit(UI_FONT_NAME), app->uiFontSize, UI_FONT_STYLE); Assert(attachResult == Result_Success);
		#endif
		
		bakeResult = TryBakeFontAtlas(&newUiFont, app->uiFontSize, UI_FONT_STYLE, 128, 1024, ArrayCount(fontCharRanges), &fontCharRanges[0]);
		if (bakeResult != Result_Success && bakeResult != Result_Partial)
		{
			Assert(bakeResult == Result_Success || bakeResult == Result_Partial);
			FreeFont(&newUiFont);
			return false;
		}
		FillFontKerningTable(&newUiFont);
		
		FontAtlas* uiAtlas = GetDefaultFontAtlas(&newUiFont);
		NotNull(uiAtlas);
		PrintLine_D("UI Atlas: %dx%d %llu glyphs lineHeight=%g maxAscend=%g maxDescend=%g centerOffset=%g fontScale=%g",
			uiAtlas->texture.Width, uiAtlas->texture.Height,
			uiAtlas->glyphs.length,
			uiAtlas->metrics.lineHeight,
			uiAtlas->metrics.maxAscend,
			uiAtlas->metrics.maxDescend,
			uiAtlas->metrics.centerOffset,
			uiAtlas->metrics.fontScale
		);
		VarArrayLoop(&uiAtlas->glyphs, gIndex)
		{
			VarArrayLoopGet(FontGlyph, glyph, &uiAtlas->glyphs, gIndex);
			if (glyph->codepoint == 'W')
			{
				PrintLine_D("W Glyph: AtlasRec=(%d, %d, %d, %d) renderOffset=(%g, %g) advanceX=%g logicalRec=(%g, %g, %g, %g)",
					glyph->atlasSourcePos.X, glyph->atlasSourcePos.Y, glyph->metrics.glyphSize.Width, glyph->metrics.glyphSize.Height,
					glyph->metrics.renderOffset.X, glyph->metrics.renderOffset.Y,
					glyph->metrics.advanceX,
					glyph->metrics.logicalRec.X, glyph->metrics.logicalRec.Y, glyph->metrics.logicalRec.Width, glyph->metrics.logicalRec.Height
				);
			}
		}
		
		MakeFontActive(&newUiFont, 128, 1024, 16, 0, 0);
	}
	
	PigFont newMainFont = ZEROED;
	{
		newMainFont = InitFont(stdHeap, StrLit("mainFont"));
		#if defined(MAIN_FONT_PATH)
		attachResult = TryAttachFontFileFromResources(&newMainFont, StrLit(MAIN_FONT_PATH), MAIN_FONT_STYLE);
		Assert(attachResult == Result_Success);
		#else //defined(MAIN_FONT_NAME)
		attachResult = TryAttachOsTtfFileToFont(&newMainFont, StrLit(MAIN_FONT_NAME), app->mainFontSize, MAIN_FONT_STYLE); Assert(attachResult == Result_Success);
		#endif
		
		bakeResult = TryBakeFontAtlas(&newMainFont, app->mainFontSize, MAIN_FONT_STYLE, 128, 1024, ArrayCount(fontCharRanges), &fontCharRanges[0]);
		if (bakeResult != Result_Success && bakeResult != Result_Partial)
		{
			Assert(bakeResult == Result_Success || bakeResult == Result_Partial);
			FreeFont(&newMainFont);
			FreeFont(&newUiFont);
			return false;
		}
		FillFontKerningTable(&newMainFont);
		
		FontAtlas* mainAtlas = GetDefaultFontAtlas(&newMainFont);
		NotNull(mainAtlas);
		PrintLine_D("Main Atlas: %dx%d %llu glyphs lineHeight=%g maxAscend=%g maxDescend=%g centerOffset=%g fontScale=%g",
			mainAtlas->texture.Width, mainAtlas->texture.Height,
			mainAtlas->glyphs.length,
			mainAtlas->metrics.lineHeight,
			mainAtlas->metrics.maxAscend,
			mainAtlas->metrics.maxDescend,
			mainAtlas->metrics.centerOffset,
			mainAtlas->metrics.fontScale
		);
		VarArrayLoop(&mainAtlas->glyphs, gIndex)
		{
			VarArrayLoopGet(FontGlyph, glyph, &mainAtlas->glyphs, gIndex);
			if (glyph->codepoint == 'W')
			{
				PrintLine_D("W Glyph: AtlasRec=(%d, %d, %d, %d) renderOffset=(%g, %g) advanceX=%g logicalRec=(%g, %g, %g, %g)",
					glyph->atlasSourcePos.X, glyph->atlasSourcePos.Y, glyph->metrics.glyphSize.Width, glyph->metrics.glyphSize.Height,
					glyph->metrics.renderOffset.X, glyph->metrics.renderOffset.Y,
					glyph->metrics.advanceX,
					glyph->metrics.logicalRec.X, glyph->metrics.logicalRec.Y, glyph->metrics.logicalRec.Width, glyph->metrics.logicalRec.Height
				);
			}
		}
		
		MakeFontActive(&newMainFont, 128, 1024, 16, 0, 0);
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

void FreeRecentFile(RecentFile* recentFile)
{
	NotNull(recentFile);
	FreeStr8(stdHeap, &recentFile->path);
	ClearPointer(recentFile);
}

void AppClearRecentFiles()
{
	VarArrayLoop(&app->recentFiles, rIndex)
	{
		VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex);
		FreeRecentFile(recentFile);
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
			LineParser parser = MakeLineParser(fileContent);
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
	Result createSettingsFolderResult = OsCreateFolder(settingsFolderPath, true);
	if (createSettingsFolderResult != Result_Success)
	{
		NotifyPrint_E("Failed to create settings folder at \"%.*s\": %s", StrPrint(settingsFolderPath), GetResultStr(createSettingsFolderResult));
		return;
	}
	
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
	else { NotifyPrint_E("Failed to save recent files list to \"%.*s\"", StrPrint(savePath)); }
	
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
			FreeRecentFile(firstFile);
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

bool AppLoadUserTheme()
{
	bool loadedSuccessfully = false;
	if (!IsEmptyStr(app->settings.userThemePath))
	{
		PrintLine_D("Loading user theme from \"%.*s\"", StrPrint(app->settings.userThemePath));
		ThemeDefinition newUserTheme;
		InitThemeDefinition(stdHeap, &newUserTheme, ThemeColor_Count);
		Result parseResult = TryLoadThemeFile(app->settings.userThemePath, &newUserTheme);
		if (parseResult == Result_Success)
		{
			FreeThemeDefinition(&app->userThemeOverrides);
			MyMemCopy(&app->userThemeOverrides, &newUserTheme, sizeof(ThemeDefinition));
			if (app->userThemeFileWatchId != 0) { RemoveFileWatch(&app->fileWatches, app->userThemeFileWatchId); }
			app->userThemeFileWatchId = AddFileWatch(&app->fileWatches, app->settings.userThemePath, CHECK_USER_THEME_PERIOD);
			loadedSuccessfully = true;
		}
		else
		{
			if (parseResult == Result_FailedToReadFile && !OsDoesFileExist(app->settings.userThemePath))
			{
				NotifyPrint_E("User theme file no longer exists at \"%.*s\"", StrPrint(app->settings.userThemePath));
				if (app->userThemeFileWatchId != 0) { RemoveFileWatch(&app->fileWatches, app->userThemeFileWatchId); app->userThemeFileWatchId = 0; }
				FreeThemeDefinition(&app->userThemeOverrides);
				SetAppSettingStr8Pntr(&app->settings, &app->settings.userThemePath, Str8_Empty);
				SaveAppSettings();
			}
			else
			{
				NotifyPrint_E("Couldn't parse theme file: %s", GetResultStr(parseResult));
			}
			FreeThemeDefinition(&newUserTheme);
		}
	}
	else
	{
		if (app->userThemeFileWatchId != 0) { RemoveFileWatch(&app->fileWatches, app->userThemeFileWatchId); app->userThemeFileWatchId = 0; }
		FreeThemeDefinition(&app->userThemeOverrides);
	}
	return loadedSuccessfully;
}

void AppBakeTheme()
{
	ScratchBegin(scratch);
	
	ThemeDefinition combinedTheme = ZEROED;
	if (!IsEmptyStr(app->settings.userThemePath))
	{
		InitThemeDefinition(scratch, &combinedTheme, MaxUXX(app->defaultTheme.entries.length, app->userThemeOverrides.entries.length));
		CombineThemeDefinitions(&app->defaultTheme, &app->userThemeOverrides, &combinedTheme);
	}
	else
	{
		MyMemCopy(&combinedTheme, &app->defaultTheme, sizeof(ThemeDefinition));
	}
	
	BakedTheme newBakedTheme = ZEROED;
	Result bakeResult = BakeTheme(&combinedTheme, app->currentThemeMode, &newBakedTheme);
	if (bakeResult == Result_Success)
	{
		MyMemCopy(&app->theme, &newBakedTheme, sizeof(BakedTheme));
	}
	else
	{
		NotifyPrint_E("Failed to bake theme: %s", GetResultStr(bakeResult));
	}
	
	ScratchEnd(scratch);
}
