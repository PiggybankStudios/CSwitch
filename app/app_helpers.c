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
			if (option->type == FileOptionType_Str) { FreeStr8(stdHeap, &option->valueStr); }
		}
		VarArrayClear(&app->fileOptions);
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
		if (StrExactStartsWith(line, StrLit("#define")) && (StrExactEndsWith(line, StrLit(" 0")) || StrExactEndsWith(line, StrLit(" 1"))))
		{
			FileOption* newOption = VarArrayAdd(FileOption, &app->fileOptions);
			NotNull(newOption);
			ClearPointer(newOption);
			newOption->name = TrimWhitespace(StrSlice(line, 7, line.length - 2));
			newOption->name = AllocStr8(stdHeap, newOption->name);
			newOption->type = FileOptionType_Bool;
			newOption->valueBool = StrExactEndsWith(line, StrLit(" 1"));
			prevOption = newOption;
		}
		else if (IsEmptyStr(line) && prevOption != nullptr && prevOption->numEmptyLinesAfter < MAX_LINE_BREAKS_CONSIDERED)
		{
			IncrementU64(prevOption->numEmptyLinesAfter);
		}
	}
	
	app->isFileOpen = true;
}

#endif //BUILD_WITH_SOKOL_GFX