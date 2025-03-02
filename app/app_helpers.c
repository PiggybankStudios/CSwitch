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

//TODO: If we see that the file write time has changed right before we go to change the file, maybe we should reload the file instead? Let external edits take precident?
void UpdateOptionValueInFile(FileOption* option)
{
	ScratchBegin(scratch);
	NotNull(option);
	NotNullStr(option->valueStr);
	if (!StrExactEquals(option->valueStr, StrSlice(app->fileContents, option->fileContentsStartIndex, option->fileContentsEndIndex)))
	{
		Str8 fileBeginning = StrSlice(app->fileContents, 0, option->fileContentsStartIndex);
		Str8 fileEnd = StrSliceFrom(app->fileContents, option->fileContentsEndIndex);
		Str8 newFileContents = JoinStringsInArena(scratch, fileBeginning, option->valueStr, false);
		newFileContents = JoinStringsInArena(scratch, newFileContents, fileEnd, false);
		bool writeResult = OsWriteTextFile(app->filePath, newFileContents);
		if (writeResult)
		{
			if (newFileContents.length != app->fileContents.length)
			{
				i64 byteOffset = (i64)newFileContents.length - (i64)app->fileContents.length;
				VarArrayLoop(&app->fileOptions, oIndex)
				{
					VarArrayLoopGet(FileOption, otherOption, &app->fileOptions, oIndex);
					if (otherOption != option && otherOption->fileContentsStartIndex >= option->fileContentsEndIndex)
					{
						otherOption->fileContentsStartIndex += byteOffset;
						otherOption->fileContentsEndIndex += byteOffset;
					}
				}
			}
			option->fileContentsEndIndex = option->fileContentsStartIndex + option->valueStr.length;
			
			FreeStr8(stdHeap, &app->fileContents);
			app->fileContents = AllocStr8(stdHeap, newFileContents);
			
			//Since we just wrote to the file, make sure we immediately updated out file write time so we don't think it was an external change
			ClearFileWatchChanged(&app->fileWatches, app->fileWatchId);
		}
		else
		{
			PrintLine_E("Failed to write %llu byte%s to file \"%.*s\"!", (u64)newFileContents.length, Plural(newFileContents.length, "s"), StrPrint(app->filePath));
			FreeStr8(stdHeap, &option->valueStr);
			option->valueStr = AllocStr8(stdHeap, StrSlice(app->fileContents, option->fileContentsStartIndex, option->fileContentsEndIndex));
		}
	}
	ScratchEnd(scratch);
}

void SetOptionValue(FileOption* option, Str8 newValueStr)
{
	NotNull(option);
	if (!StrExactEquals(option->valueStr, newValueStr))
	{
		FreeStr8(stdHeap, &option->valueStr);
		option->valueStr = AllocStr8(stdHeap, newValueStr);
		UpdateOptionValueInFile(option);
	}
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

Str8 GetUniqueDisplayPath(FilePath filePath)
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
		
		if (!foundConflict)
		{
			return subPath;
		}
	}
	return filePath;
	
}

void AppCloseFile()
{
	if (app->isFileOpen)
	{
		RemoveFileWatch(&app->fileWatches, app->fileWatchId);
		app->fileWatchId = 0;
		FreeStr8(stdHeap, &app->filePath);
		FreeStr8WithNt(stdHeap, &app->fileContents);
		VarArrayLoop(&app->fileOptions, oIndex)
		{
			VarArrayLoopGet(FileOption, option, &app->fileOptions, oIndex);
			FreeStr8(stdHeap, &option->name);
			FreeStr8(stdHeap, &option->valueStr);
		}
		VarArrayClear(&app->fileOptions);
		platform->SetWindowTitle(StrLit(PROJECT_READABLE_NAME_STR));
		app->isFileOpen = false;
	}
}

void AppOpenFile(FilePath filePath)
{
	Str8 fileContents = Str8_Empty;
	bool openResult = OsReadTextFile(filePath, stdHeap, &fileContents);
	if (!openResult) { PrintLine_E("Failed to open file at \"%.*s\"", StrPrint(filePath)); return; }
	
	if (app->isFileOpen) { AppCloseFile(); }
	app->fileContents = fileContents;
	app->filePath = AllocStr8(stdHeap, filePath);
	
	LineParser lineParser = NewLineParser(fileContents);
	Str8 fullLine = Str8_Empty;
	FileOption* prevOption = nullptr;
	while (LineParserGetLine(&lineParser, &fullLine))
	{
		Str8 line = TrimWhitespace(fullLine);
		uxx lineStartIndex = lineParser.lineBeginByteIndex + (uxx)(line.chars - fullLine.chars);
		uxx lineEndIndex = lineStartIndex + line.length;
		const char* possibleBoolValues[] = { "1", "0", "true", "false" }; //NOTE: These must alternate truthy/falsey so %2 logic below works
		bool isOption = false;
		Str8 defineStr = StrLit("#define");
		for (uxx vIndex = 0; vIndex < ArrayCount(possibleBoolValues); vIndex++)
		{
			Str8 boolValueStr = StrLit(possibleBoolValues[vIndex]);
			if (line.length >= defineStr.length + 1 + boolValueStr.length &&
				StrExactStartsWith(line, defineStr) &&
				StrExactEndsWith(line, boolValueStr) &&
				IsCharWhitespace(line.chars[line.length-boolValueStr.length-1], false))
			{
				FileOption* newOption = VarArrayAdd(FileOption, &app->fileOptions);
				NotNull(newOption);
				ClearPointer(newOption);
				newOption->name = TrimWhitespace(StrSlice(line, defineStr.length, line.length - boolValueStr.length));
				newOption->name = AllocStr8(stdHeap, newOption->name);
				newOption->type = FileOptionType_Bool;
				newOption->valueBool = ((vIndex%2) == 0);
				newOption->fileContentsStartIndex = lineEndIndex - boolValueStr.length;
				newOption->fileContentsEndIndex = lineEndIndex;
				newOption->valueStr = AllocStr8(stdHeap, StrSlice(app->fileContents, newOption->fileContentsStartIndex, newOption->fileContentsEndIndex));
				prevOption = newOption;
				isOption = true;
				break;
			}
		}
		
		if (!isOption && IsEmptyStr(line) && prevOption != nullptr && prevOption->numEmptyLinesAfter < MAX_LINE_BREAKS_CONSIDERED)
		{
			IncrementU64(prevOption->numEmptyLinesAfter);
		}
	}
	
	platform->SetWindowTitle(ScratchPrintStr("%.*s - %s", StrPrint(app->filePath), PROJECT_READABLE_NAME_STR));
	app->isFileOpen = true;
	
	app->fileWatchId = AddFileWatch(&app->fileWatches, app->filePath, CHECK_FILE_WRITE_TIME_PERIOD);
	
	AppRememberRecentFile(filePath);
}

bool AppCheckForFileChanges()
{
	bool didFileChange = false;
	if (HasFileWatchChangedWithDelay(&app->fileWatches, app->fileWatchId, FILE_RELOAD_DELAY))
	{
		ClearFileWatchChanged(&app->fileWatches, app->fileWatchId);
		WriteLine_N("File changed externally! Reloading...");
		ScratchBegin(scratch);
		//NOTE: We have to allocate the string in scratch because AppOpenFile is going to deallocate app->filePath before it allocates the new one
		Str8 filePathAlloc = AllocStrAndCopy(scratch, app->filePath.length, app->filePath.chars, false);
		AppOpenFile(filePathAlloc);
		ScratchEnd(scratch);
		didFileChange = true;
	}
	return didFileChange;
}
