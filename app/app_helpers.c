/*
File:   app_helpers.c
Author: Taylor Robbins
Date:   02\25\2025
Description: 
	** None
*/

#if BUILD_WITH_SOKOL_GFX

ImageData LoadImageData(Arena* arena, const char* path)
{
	ScratchBegin1(scratch, arena);
	Slice fileContents = Slice_Empty;
	bool readFileResult = OsReadFile(FilePathLit(path), scratch, false, &fileContents);
	Assert(readFileResult);
	ImageData imageData = ZEROED;
	Result parseResult = TryParseImageFile(fileContents, arena, &imageData);
	Assert(parseResult == Result_Success);
	ScratchEnd(scratch);
	return imageData;
}

#if BUILD_WITH_SOKOL_APP
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
#endif //BUILD_WITH_SOKOL_APP

void AppCloseFile()
{
	if (app->isFileOpen)
	{
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
		if (StrExactStartsWith(line, StrLit("#define")) && (StrExactEndsWith(line, StrLit(" 0")) || StrExactEndsWith(line, StrLit(" 1"))))
		{
			FileOption* newOption = VarArrayAdd(FileOption, &app->fileOptions);
			NotNull(newOption);
			ClearPointer(newOption);
			newOption->name = TrimWhitespace(StrSlice(line, 7, line.length - 2));
			newOption->name = AllocStr8(stdHeap, newOption->name);
			newOption->type = FileOptionType_Bool;
			newOption->valueBool = StrExactEndsWith(line, StrLit(" 1"));
			newOption->fileContentsStartIndex = lineEndIndex-1;
			newOption->fileContentsEndIndex = lineEndIndex;
			newOption->valueStr = AllocStr8(stdHeap, StrSlice(app->fileContents, newOption->fileContentsStartIndex, newOption->fileContentsEndIndex));
			prevOption = newOption;
		}
		else if (IsEmptyStr(line) && prevOption != nullptr && prevOption->numEmptyLinesAfter < MAX_LINE_BREAKS_CONSIDERED)
		{
			IncrementU64(prevOption->numEmptyLinesAfter);
		}
	}
	
	platform->SetWindowTitle(ScratchPrintStr("%.*s - %s", StrPrint(app->filePath), PROJECT_READABLE_NAME_STR));
	app->isFileOpen = true;
	
	Result getWriteTimeResult = OsGetFileWriteTime(filePath, &app->fileWriteTime);
	if (getWriteTimeResult == Result_Success)
	{
		app->gotFileWriteTime = true;
		app->lastWriteTimeCheck = appIn->programTime;
	}
	else
	{
		app->gotFileWriteTime = false;
		PrintLine_W("Failed to get file write time for \"%.*s\": %s", StrPrint(filePath), GetResultStr(getWriteTimeResult));
		WriteLine_W("We will not be able to detect external file changes, you may lose work if you edit the file while this program is open");
	}
}

bool CheckForFileChanges()
{
	bool didFileChange = false;
	if (app->isFileOpen && app->gotFileWriteTime)
	{
		if (TimeSinceBy(appIn->programTime, app->lastWriteTimeCheck) > CHECK_FILE_WRITE_TIME_PERIOD)
		{
			OsFileWriteTime newWriteTime = ZEROED;
			Result getWriteTimeResult = OsGetFileWriteTime(app->filePath, &newWriteTime);
			if (getWriteTimeResult == Result_Success)
			{
				if (MyMemCompare(&newWriteTime, &app->fileWriteTime, sizeof(OsFileWriteTime)) != 0)
				{
					WriteLine_N("File changed externally! Reloading...");
					ScratchBegin(scratch);
					//NOTE: We have to allocate the string in scratch because AppOpenFile is going to deallocate app->filePath before it allocates the new one
					Str8 filePathAlloc = AllocStrAndCopy(scratch, app->filePath.length, app->filePath.chars, false);
					AppOpenFile(filePathAlloc);
					ScratchEnd(scratch);
					didFileChange = true;
				}
			}
			else
			{
				app->gotFileWriteTime = false;
				PrintLine_W("Failed to get file write time for \"%.*s\": %s", StrPrint(app->filePath), GetResultStr(getWriteTimeResult));
				WriteLine_W("We will not be able to detect external file changes, you may lose work if you edit the file while this program is open");
			}
		}
	}
	return didFileChange;
}

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
					if (otherOption != option && otherOption->fileContentsStartIndex >= option->fileContentsEndIndex) { option->fileContentsStartIndex += byteOffset; option->fileContentsEndIndex += byteOffset; }
				}
			}
			option->fileContentsEndIndex = option->fileContentsStartIndex + option->valueStr.length;
			
			FreeStr8(stdHeap, &app->fileContents);
			app->fileContents = AllocStr8(stdHeap, newFileContents);
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

#endif //BUILD_WITH_SOKOL_GFX