/*
File:   app_main.h
Author: Taylor Robbins
Date:   02\25\2025
*/

#ifndef _APP_MAIN_H
#define _APP_MAIN_H

typedef enum FileOptionType FileOptionType;
enum FileOptionType
{
	FileOptionType_None = 0,
	FileOptionType_Bool,
	FileOptionType_I32,
	FileOptionType_R32,
	FileOptionType_Str,
	FileOptionType_Count,
};
const char* GetFileOptionTypeStr(FileOptionType enumValue)
{
	switch (enumValue)
	{
		case FileOptionType_None:  return "None";
		case FileOptionType_Bool:  return "Bool";
		case FileOptionType_I32:   return "I32";
		case FileOptionType_R32:   return "R32";
		case FileOptionType_Str:   return "Str";
		default: return "Unknown";
	}
}

typedef struct FileOption FileOption;
struct FileOption
{
	Str8 name;
	FileOptionType type;
	u64 numEmptyLinesAfter;
	Str8 valueStr;
	uxx fileContentsStartIndex;
	uxx fileContentsEndIndex;
	union
	{
		bool valueBool;
		i32 valueI32;
		r32 valueR32;
	};
};

typedef struct RecentFile RecentFile;
struct RecentFile
{
	Str8 path;
	bool fileExists;
};

typedef struct AppData AppData;
struct AppData
{
	bool initialized;
	RandomSeries random;
	AppResources resources;
	
	Shader mainShader;
	Font uiFont;
	Font mainFont;
	
	u64 numFramesConsecutivelyRendered;
	
	ClayUIRenderer clay;
	u16 clayUiFontId;
	u16 clayMainFontId;
	bool isFileMenuOpen;
	bool isWindowMenuOpen;
	bool isOpenRecentSubmenuOpen;
	
	bool isFileOpen;
	Str8 filePath;
	Str8 fileContents;
	OsFileWriteTime fileWriteTime;
	bool gotFileWriteTime;
	u64 lastWriteTimeCheck;
	VarArray fileOptions; //FileOption
	
	VarArray recentFiles; //RecentFile
};

#endif //  _APP_MAIN_H
