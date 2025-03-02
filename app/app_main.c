/*
File:   app_main.c
Author: Taylor Robbins
Date:   01\19\2025
Description: 
	** Contains the dll entry point and all exported functions that the platform
	** layer can lookup by name and call. Also includes all other source files
	** required for the application to compile.
*/

#include "build_config.h"
#include "defines.h"
#define PIG_CORE_IMPLEMENTATION BUILD_INTO_SINGLE_UNIT

#include "base/base_all.h"
#include "std/std_all.h"
#include "os/os_all.h"
#include "mem/mem_all.h"
#include "struct/struct_all.h"
#include "misc/misc_all.h"
#include "input/input_all.h"
#include "file_fmt/file_fmt_all.h"
#include "ui/ui_all.h"
#include "gfx/gfx_all.h"
#include "gfx/gfx_system_global.h"
#include "phys/phys_all.h"

// +--------------------------------------------------------------+
// |                         Header Files                         |
// +--------------------------------------------------------------+
#include "platform_interface.h"
#include "app_resources.h"
#include "app_main.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
static AppData* app = nullptr;
static AppInput* appIn = nullptr;

#if !BUILD_INTO_SINGLE_UNIT //NOTE: The platform layer already has these globals
static PlatformInfo* platformInfo = nullptr;
static PlatformApi* platform = nullptr;
static Arena* stdHeap = nullptr;
#endif

// +--------------------------------------------------------------+
// |                         Source Files                         |
// +--------------------------------------------------------------+
#include "main2d_shader.glsl.h"
#include "app_resources.c"
#include "app_helpers.c"
#include "app_clay.c"

// +==============================+
// |           DllMain            |
// +==============================+
#if (TARGET_IS_WINDOWS && !BUILD_INTO_SINGLE_UNIT)
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, // handle to DLL module
	DWORD fdwReason,    // reason for calling function
	LPVOID lpReserved)
{
	UNUSED(hinstDLL);
	UNUSED(lpReserved);
	switch(fdwReason)
	{ 
		case DLL_PROCESS_ATTACH: break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
		default: break;
	}
	//If we don't return TRUE here then the LoadLibraryA call will return a failure!
	return TRUE;
}
#endif //(TARGET_IS_WINDOWS && !BUILD_INTO_SINGLE_UNIT)

void UpdateDllGlobals(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr, AppInput* appInput)
{
	#if !BUILD_INTO_SINGLE_UNIT
	platformInfo = inPlatformInfo;
	platform = inPlatformApi;
	stdHeap = inPlatformInfo->platformStdHeap;
	#else
	UNUSED(inPlatformApi);
	UNUSED(inPlatformInfo);
	#endif
	app = (AppData*)memoryPntr;
	appIn = appInput;
}

// +==============================+
// |           AppInit            |
// +==============================+
// void* AppInit(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi)
EXPORT_FUNC(AppInit) APP_INIT_DEF(AppInit)
{
	#if !BUILD_INTO_SINGLE_UNIT
	InitScratchArenasVirtual(Gigabytes(4));
	#endif
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	
	AppData* appData = AllocType(AppData, inPlatformInfo->platformStdHeap);
	ClearPointer(appData);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, (void*)appData, nullptr);
	
	InitAppResources(&app->resources);
	
	platform->SetWindowTitle(StrLit(PROJECT_READABLE_NAME_STR));
	LoadWindowIcon();
	
	InitRandomSeriesDefault(&app->random);
	SeedRandomSeriesU64(&app->random, OsGetCurrentTimestamp(false));
	
	InitCompiledShader(&app->mainShader, stdHeap, main2d);
	
	FontCharRange fontCharRanges[] = { FontCharRange_ASCII, FontCharRange_LatinExt, };
	{
		app->uiFont = InitFont(stdHeap, StrLit("uiFont"));
		Result attachResult = AttachOsTtfFileToFont(&app->uiFont, StrLit(UI_FONT_NAME), UI_FONT_SIZE, UI_FONT_STYLE);
		Assert(attachResult == Result_Success);
		Result bakeResult = BakeFontAtlas(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE, NewV2i(256, 256), ArrayCount(fontCharRanges), &fontCharRanges[0]);
		Assert(bakeResult == Result_Success);
		FillFontKerningTable(&app->uiFont);
		RemoveAttachedTtfFile(&app->uiFont);
	}
	
	{
		app->mainFont = InitFont(stdHeap, StrLit("mainFont"));
		Result attachResult = AttachOsTtfFileToFont(&app->mainFont, StrLit(MAIN_FONT_NAME), MAIN_FONT_SIZE, MAIN_FONT_STYLE);
		Assert(attachResult == Result_Success);
		
		Result bakeResult = BakeFontAtlas(&app->mainFont, MAIN_FONT_SIZE, MAIN_FONT_STYLE, NewV2i(256, 256), ArrayCount(fontCharRanges), &fontCharRanges[0]);
		Assert(bakeResult == Result_Success);
		FillFontKerningTable(&app->mainFont);
		RemoveAttachedTtfFile(&app->mainFont);
	}
	
	InitClayUIRenderer(stdHeap, V2_Zero, &app->clay);
	app->clayUiFontId = AddClayUIRendererFont(&app->clay, &app->uiFont, UI_FONT_STYLE);
	app->clayMainFontId = AddClayUIRendererFont(&app->clay, &app->mainFont, MAIN_FONT_STYLE);
	
	InitVarArray(FileOption, &app->fileOptions, stdHeap);
	
	uxx argIndex = 0;
	Str8 pathArgument = GetNamelessProgramArg(platformInfo->programArgs, argIndex);
	while (!IsEmptyStr(pathArgument))
	{
		if (OsDoesFileExist(pathArgument))
		{
			AppOpenFile(pathArgument);
			if (app->isFileOpen) { break; }
		}
		
		argIndex++;
		GetNamelessProgramArg(platformInfo->programArgs, argIndex);
	}
	
	InitVarArray(RecentFile, &app->recentFiles, stdHeap);
	AppLoadRecentFilesList();
	if (app->recentFiles.length > 0)
	{
		RecentFile* mostRecentFile = VarArrayGetLast(RecentFile, &app->recentFiles);
		AppOpenFile(mostRecentFile->path);
	}
	
	app->initialized = true;
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
	return (void*)app;
}

// +==============================+
// |          AppUpdate           |
// +==============================+
// bool AppUpdate(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr, AppInput* appInput)
EXPORT_FUNC(AppUpdate) APP_UPDATE_DEF(AppUpdate)
{
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	bool shouldContinueRunning = true;
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, appInput);
	
	
	bool refreshScreen = false;
	if (CheckForFileChanges()) { refreshScreen = true; }
	if (!AreEqual(appIn->mouse.prevPosition, appIn->mouse.position) && (appIn->mouse.isOverWindow || appIn->mouse.wasOverWindow)) { refreshScreen = true; }
	if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Left) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Left)) { refreshScreen = true; }
	if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Right) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Right)) { refreshScreen = true; }
	if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Middle) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Middle)) { refreshScreen = true; }
	if (appIn->isFullscreenChanged || appIn->isMinimizedChanged || appIn->isFocusedChanged || appIn->screenSizeChanged) { refreshScreen = true; }
	if (appIn->mouse.scrollDelta.X != 0 || appIn->mouse.scrollDelta.Y != 0) { refreshScreen = true; }
	if (refreshScreen) { app->numFramesConsecutivelyRendered = 0; }
	else { app->numFramesConsecutivelyRendered++; }
	if (!refreshScreen && app->numFramesConsecutivelyRendered >= NUM_FRAMES_BEFORE_SLEEP)
	{
		ScratchEnd(scratch);
		ScratchEnd(scratch2);
		ScratchEnd(scratch3);
		return shouldContinueRunning;
	}
	
	v2i screenSizei = appIn->screenSize;
	v2 screenSize = ToV2Fromi(appIn->screenSize);
	// v2 screenCenter = Div(screenSize, 2.0f);
	v2 mousePos = appIn->mouse.position;
	
	if (appIn->droppedFilePaths.length > 0)
	{
		if (appIn->droppedFilePaths.length > 1) { PrintLine_W("WARNING: Dropped %llu files at the same time. We only support opening 1 file at a time!", (u64)appIn->droppedFilePaths.length); }
		Str8 droppedFilePath = *VarArrayGetFirst(Str8, &appIn->droppedFilePaths);
		PrintLine_I("Dropped file: \"%.*s\"", StrPrint(droppedFilePath));
		AppOpenFile(droppedFilePath);
	}
	
	if (IsMouseBtnPressed(&appIn->mouse, MouseBtn_Right))
	{
		Str8 appDataPath = OsGetSettingsSavePath(scratch, Str8_Empty, StrLit(PROJECT_FOLDER_NAME_STR), true);
		PrintLine_D("appDataPath: \"%.*s\"", StrPrint(appDataPath));
	}
	
	BeginFrame(platform->GetSokolSwapchain(), screenSizei, BACKGROUND_BLACK, 1.0f);
	{
		BindShader(&app->mainShader);
		ClearDepthBuffer(1.0f);
		SetDepth(1.0f);
		mat4 projMat = Mat4_Identity;
		TransformMat4(&projMat, MakeScaleXYZMat4(1.0f/(screenSize.Width/2.0f), 1.0f/(screenSize.Height/2.0f), 1.0f));
		TransformMat4(&projMat, MakeTranslateXYZMat4(-1.0f, -1.0f, 0.0f));
		TransformMat4(&projMat, MakeScaleYMat4(-1.0f));
		SetProjectionMat(projMat);
		SetViewMat(Mat4_Identity);
		
		BeginClayUIRender(&app->clay.clay, screenSize, 16.6f, false, mousePos, IsMouseBtnDown(&appIn->mouse, MouseBtn_Left), appIn->mouse.scrollDelta);
		{
			CLAY({ .id = CLAY_ID("FullscreenContainer"),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
				},
			})
			{
				// +==============================+
				// |            Topbar            |
				// +==============================+
				CLAY({ .id = CLAY_ID("Topbar"),
					.layout = {
						.sizing = {
							.height = CLAY_SIZING_FIXED(TOPBAR_HEIGHT),
							.width = CLAY_SIZING_GROW(0),
						},
						.padding = { 0, 0, 0, 0 },
						.childGap = 2,
						.childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
					},
					.backgroundColor = ToClayColor(BACKGROUND_GRAY),
					.border = { .color=ToClayColor(OUTLINE_GRAY), .width={ .bottom=1 } },
				})
				{
					if (ClayTopBtn("File", &app->isFileMenuOpen, app->isOpenRecentSubmenuOpen, FILE_DROPDOWN_WIDTH))
					{
						if (ClayBtn("Open", true))
						{
							Str8 selectedPath = Str8_Empty;
							Result openResult = OsDoOpenFileDialog(scratch, &selectedPath);
							if (openResult == Result_Success)
							{
								PrintLine_I("Opened \"%.*s\"", StrPrint(selectedPath));
								AppOpenFile(selectedPath);
							}
							else if (openResult == Result_Canceled) { WriteLine_D("Canceled..."); }
							else { PrintLine_E("OpenDialog error: %s", GetResultStr(openResult)); }
						} Clay__CloseElement();
						
						if (app->recentFiles.length > 0)
						{
							if (ClayTopSubmenu("Open Recent", app->isFileMenuOpen, &app->isOpenRecentSubmenuOpen, OPEN_RECENT_DROPDOWN_WIDTH))
							{
								for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
								{
									RecentFile* recentFile = VarArrayGet(RecentFile, &app->recentFiles, rIndex-1);
									Str8 fileName = GetUniqueDisplayPath(recentFile->path);
									bool isOpenFile = (app->isFileOpen && StrAnyCaseEquals(app->filePath, recentFile->path));
									if (ClayBtnStrEx(recentFile->path, ScratchPrintStr("%.*s", StrPrint(fileName)), !isOpenFile && recentFile->fileExists))
									{
										AppOpenFile(recentFile->path);
										app->isOpenRecentSubmenuOpen = false;
										app->isFileMenuOpen = false;
									} Clay__CloseElement();
								}
								
								if (ClayBtn("Clear Recent Files", app->recentFiles.length > 0))
								{
									AppClearRecentFiles();
									app->isOpenRecentSubmenuOpen = false;
									app->isFileMenuOpen = false;
								} Clay__CloseElement();
								
								Clay__CloseElement();
								Clay__CloseElement();
							} Clay__CloseElement();
						}
						else
						{
							if (ClayBtn("Open Recent", false)) { } Clay__CloseElement();
						}
						
						if (ClayBtn("Close", app->isFileOpen))
						{
							AppCloseFile();
						} Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					if (ClayTopBtn("Window", &app->isWindowMenuOpen, false, WINDOW_DROPDOWN_WIDTH))
					{
						if (ClayBtnStr(ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "- Disable" : "+ Enable"), TARGET_IS_WINDOWS))
						{
							platform->SetWindowTopmost(!appIn->isWindowTopmost);
						} Clay__CloseElement();
						
						if (ClayBtn("Close", true))
						{
							shouldContinueRunning = false;
						} Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					CLAY({ .layout = { .sizing = { .width=CLAY_SIZING_GROW(0) } } }) {}
					
					if (app->isFileOpen)
					{
						Str8 filePathTrimmed = app->filePath;
						if (filePathTrimmed.length > FILE_PATH_DISPLAY_MAX_LENGTH)
						{
							Str8 fileNamePart = GetFileNamePart(app->filePath, true);
							uxx fileNameStartIndex = (uxx)(fileNamePart.chars - app->filePath.chars);
							uxx ellipsesPos = (fileNameStartIndex > 0) ? (fileNameStartIndex/2) : (filePathTrimmed.length/2);
							uxx numCharsToCut = filePathTrimmed.length - 3 - FILE_PATH_DISPLAY_MAX_LENGTH;
							if (ellipsesPos > numCharsToCut/2)
							{
								Str8 firstPart = StrSlice(filePathTrimmed, 0, ellipsesPos - numCharsToCut/2);
								Str8 secondPart = StrSliceFrom(filePathTrimmed, ellipsesPos + (numCharsToCut+1)/2);
								filePathTrimmed = PrintInArenaStr(scratch, "%.*s...%.*s", StrPrint(firstPart), StrPrint(secondPart));
							}
							else
							{
								Str8 lastPart = StrSliceFrom(filePathTrimmed, filePathTrimmed.length - FILE_PATH_DISPLAY_MAX_LENGTH + 3);
								filePathTrimmed = PrintInArenaStr(scratch, "...%.*s", StrPrint(lastPart));
							}
						}
						CLAY_TEXT(
							ToClayString(filePathTrimmed),
							CLAY_TEXT_CONFIG({
								.fontId = app->clayUiFontId,
								.fontSize = UI_FONT_SIZE,
								.textColor = ToClayColor(TEXT_LIGHT_GRAY),
							})
						);
						CLAY({ .layout={ .sizing={ .width=CLAY_SIZING_FIXED(4) } } }) {}
					}
				}
				
				// +==============================+
				// |         Options List         |
				// +==============================+
				CLAY({
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.childGap = OPTION_UI_GAP,
						.padding = CLAY_PADDING_ALL(4),
						.sizing = {
							.height=CLAY_SIZING_GROW(0),
							.width=CLAY_SIZING_GROW(0)
						},
					},
					.scroll = { .vertical=true },
				})
				{
					VarArrayLoop(&app->fileOptions, oIndex)
					{
						VarArrayLoopGet(FileOption, option, &app->fileOptions, oIndex);
						if (option->type == FileOptionType_Bool)
						{
							//NOTE: We have to put a copy of valueStr in scratch because the current valueStr might be deallocated before the end of the frame when Clay needs to render the text!
							if (ClayOptionBtn(option->name, ScratchPrintStr("%.*s", StrPrint(option->valueStr)), option->valueBool))
							{
								option->valueBool = !option->valueBool;
								if (StrExactEquals(option->valueStr, StrLit("false")))
								{
									SetOptionValue(option, StrLit("true"));
								}
								else if (StrExactEquals(option->valueStr, StrLit("true")))
								{
									SetOptionValue(option, StrLit("false"));
								}
								else if (StrExactEquals(option->valueStr, StrLit("0")))
								{
									SetOptionValue(option, StrLit("1"));
								}
								else
								{
									SetOptionValue(option, StrLit("0"));
								}
							} Clay__CloseElement();
						}
						else
						{
							if (ClayOptionBtn(option->name, StrLit("-"), false))
							{
								//TODO: Implement me!
							} Clay__CloseElement();
						}
						if (option->numEmptyLinesAfter > 0)
						{
							CLAY({ .layout = { .sizing = { .height=CLAY_SIZING_FIXED((r32)option->numEmptyLinesAfter * LINE_BREAK_EXTRA_UI_GAP) } } }) {}
						}
					}
					
					#if 0
					if (app->isFileOpen)
					{
						CLAY_TEXT(
							ToClayString(app->fileContents),
							CLAY_TEXT_CONFIG({
								.fontId = app->clayMainFontId,
								.fontSize = MAIN_FONT_SIZE,
								.textColor = ToClayColor(TEXT_WHITE),
								.wrapMode = CLAY_TEXT_WRAP_NEWLINES,
							})
						);
					}
					#endif
				}
			}
		}
		Clay_RenderCommandArray clayRenderCommands = EndClayUIRender(&app->clay.clay);
		RenderClayCommandArray(&app->clay, &gfx, &clayRenderCommands);
	}
	EndFrame();
	
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
	return shouldContinueRunning;
}

// +==============================+
// |          AppClosing          |
// +==============================+
// void AppClosing(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr)
EXPORT_FUNC(AppClosing) APP_CLOSING_DEF(AppClosing)
{
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, nullptr);
	
	#if BUILD_WITH_IMGUI
	igSaveIniSettingsToDisk(app->imgui->io->IniFilename);
	#endif
	
	AppSaveRecentFilesList();
	
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
}

// +==============================+
// |          AppGetApi           |
// +==============================+
// AppApi AppGetApi()
EXPORT_FUNC(AppGetApi) APP_GET_API_DEF(AppGetApi)
{
	AppApi result = ZEROED;
	result.AppInit = AppInit;
	result.AppUpdate = AppUpdate;
	result.AppClosing = AppClosing;
	return result;
}
