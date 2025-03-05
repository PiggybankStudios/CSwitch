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
#include "app_icons.h"
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
#include "app_file_watch.c"
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
	
	for (uxx iIndex = 1; iIndex < AppIcon_Count; iIndex++)
	{
		AppIcon iconEnum = (AppIcon)iIndex;
		const char* iconPath = GetAppIconPath(iconEnum);
		if (iconPath != nullptr)
		{
			ImageData iconImageData = LoadImageData(scratch, iconPath);
			app->icons[iIndex] = InitTexture(stdHeap, StrLit(GetAppIconStr(iconEnum)), iconImageData.size, iconImageData.pixels, TextureFlag_NoMipmaps);
			Assert(app->icons[iIndex].error == Result_Success);
		}
	}
	
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
	
	InitFileWatches(&app->fileWatches);
	InitVarArray(FileOption, &app->fileOptions, stdHeap);
	
	InitVarArray(RecentFile, &app->recentFiles, stdHeap);
	AppLoadRecentFilesList();
	
	bool wasCmdPathGiven = false;
	//NOTE: Not really sure if we need to handle multiple argument paths being passed.
	// I guess if the first one fails doesn't point to a real file we can
	// open secondary one(s) but that really isn't super intuitive behavior
	uxx argIndex = 0;
	Str8 pathArgument = GetNamelessProgramArg(platformInfo->programArgs, argIndex);
	while (!IsEmptyStr(pathArgument))
	{
		wasCmdPathGiven = true;
		if (OsDoesFileExist(pathArgument))
		{
			AppOpenFile(pathArgument);
			if (app->isFileOpen) { break; }
		}
		else
		{
			PrintLine_E("Command line path does not point to a file: \"%.*s\"", StrPrint(pathArgument));
		}
		
		argIndex++;
		pathArgument = GetNamelessProgramArg(platformInfo->programArgs, argIndex);
	}
	
	if (!wasCmdPathGiven && app->recentFiles.length > 0)
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
	
	UpdateFileWatches(&app->fileWatches);
	
	bool refreshScreen = false;
	if (app->isFileOpen && AppCheckForFileChanges()) { refreshScreen = true; }
	if (app->recentFilesWatchId != 0 && HasFileWatchChangedWithDelay(&app->fileWatches, app->recentFilesWatchId, RECENT_FILES_RELOAD_DELAY))
	{
		ClearFileWatchChanged(&app->fileWatches, app->recentFilesWatchId);
		AppLoadRecentFilesList();
		refreshScreen = true;
	}
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
					if (ClayTopBtn("File", &app->isFileMenuOpen, app->isOpenRecentSubmenuOpen))
					{
						if (ClayBtn("Open...", true, &app->icons[AppIcon_OpenFile]))
						{
							Str8 selectedPath = Str8_Empty;
							Result openResult = OsDoOpenFileDialog(scratch, &selectedPath);
							if (openResult == Result_Success)
							{
								PrintLine_I("Opened \"%.*s\"", StrPrint(selectedPath));
								AppOpenFile(selectedPath);
								app->isFileMenuOpen = false;
							}
							else if (openResult == Result_Canceled) { WriteLine_D("Canceled..."); }
							else { PrintLine_E("OpenDialog error: %s", GetResultStr(openResult)); }
						} Clay__CloseElement();
						
						if (app->recentFiles.length > 0)
						{
							if (ClayTopSubmenu("Open Recent >", app->isFileMenuOpen, &app->isOpenRecentSubmenuOpen, OPEN_RECENT_DROPDOWN_WIDTH, &app->icons[AppIcon_OpenRecent]))
							{
								for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
								{
									RecentFile* recentFile = VarArrayGet(RecentFile, &app->recentFiles, rIndex-1);
									Str8 fileName = GetUniqueDisplayPath(recentFile->path);
									bool isOpenFile = (app->isFileOpen && StrAnyCaseEquals(app->filePath, recentFile->path));
									if (ClayBtnStrEx(recentFile->path, ScratchPrintStr("%.*s", StrPrint(fileName)), !isOpenFile && recentFile->fileExists, nullptr))
									{
										AppOpenFile(recentFile->path);
										app->isOpenRecentSubmenuOpen = false;
										app->isFileMenuOpen = false;
									} Clay__CloseElement();
								}
								
								if (ClayBtn("Clear Recent Files", app->recentFiles.length > 0, &app->icons[AppIcon_Trash]))
								{
									AppClearRecentFiles();
									AppSaveRecentFilesList();
									app->isOpenRecentSubmenuOpen = false;
									app->isFileMenuOpen = false;
								} Clay__CloseElement();
								
								Clay__CloseElement();
								Clay__CloseElement();
							} Clay__CloseElement();
						}
						else
						{
							if (ClayBtn("Open Recent >", false, &app->icons[AppIcon_OpenRecent])) { } Clay__CloseElement();
						}
						
						if (ClayBtn("Close File", app->isFileOpen, &app->icons[AppIcon_CloseFile]))
						{
							AppCloseFile();
							app->isFileMenuOpen = false;
						} Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					if (ClayTopBtn("Window", &app->isWindowMenuOpen, false))
					{
						if (ClayBtnStr(ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "Disable" : "Enable"), TARGET_IS_WINDOWS, &app->icons[appIn->isWindowTopmost ? AppIcon_TopmostEnabled : AppIcon_TopmostDisabled]))
						{
							platform->SetWindowTopmost(!appIn->isWindowTopmost);
						} Clay__CloseElement();
						
						#if DEBUG_BUILD
						if (ClayBtn(ScratchPrint("%s Clay UI Debug", Clay_IsDebugModeEnabled() ? "Hide" : "Show"), true, &app->icons[AppIcon_Debug]))
						{
							Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
						} Clay__CloseElement();
						#endif
						
						if (ClayBtn("Close Window", true, &app->icons[AppIcon_CloseWindow]))
						{
							shouldContinueRunning = false;
							app->isFileMenuOpen = false;
						} Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					// CLAY({ .layout = { .sizing = { .width=CLAY_SIZING_GROW(0) } } }) {}
					
					if (app->isFileOpen)
					{
						CLAY({ .id=CLAY_ID("FilePathArea"), .layout = { .sizing = { .width=CLAY_SIZING_GROW(0) } } })
						{
							rec availableRec = GetClayElementDrawRec(CLAY_ID("FilePathArea"));
							uxx numCharsEstimate = UINTXX_MAX;
							if (availableRec.Width > 0)
							{
								v2 charSize = MeasureTextEx(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE, StrLit("W")).visualRec.Size;
								numCharsEstimate = (uxx)MaxI32(1, FloorR32i(availableRec.Width / charSize.Width));
							}
							Str8 filePathTrimmed = ShortenFilePath(scratch, app->filePath, numCharsEstimate, StrLit("..."));
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
				}
				
				CLAY({
					.layout = {
						.layoutDirection = CLAY_LEFT_TO_RIGHT,
						.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
					}
				})
				{
					// +==============================+
					// |         Options List         |
					// +==============================+
					CLAY({ .id = CLAY_ID("OptionsList"),
						.layout = {
							.layoutDirection = CLAY_TOP_TO_BOTTOM,
							.childGap = OPTION_UI_GAP,
							.padding = CLAY_PADDING_ALL(4),
							.sizing = {
								.height = CLAY_SIZING_GROW(0),
								.width = CLAY_SIZING_GROW(0)
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
							else if (option->type == FileOptionType_CommentDefine)
							{
								if (ClayOptionBtn(ScratchPrintStr("%s%.*s", option->isUncommented ? "" : "// ", StrPrint(option->name)), Str8_Empty, option->isUncommented))
								{
									option->isUncommented = !option->isUncommented;
									SetOptionValue(option, StrLit(option->isUncommented ? "" : "// "));
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
					}
					
					rec optionsListDrawRec = GetClayElementDrawRec(CLAY_ID("OptionsList"));
					Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"));
					r32 scrollbarYPercent = 0.0f;
					r32 scrollbarSizePercent = 1.0f;
					if (optionsListScrollData.found && optionsListScrollData.contentDimensions.height > optionsListScrollData.scrollContainerDimensions.height)
					{
						scrollbarSizePercent = ClampR32(optionsListScrollData.scrollContainerDimensions.height / optionsListScrollData.contentDimensions.height, 0.0f, 1.0f);
						scrollbarYPercent = ClampR32(-optionsListScrollData.scrollPosition->y / (optionsListScrollData.contentDimensions.height - optionsListScrollData.scrollContainerDimensions.height), 0.0f, 1.0f);
					}
					
					if (optionsListScrollData.found && scrollbarSizePercent < 1.0f)
					{
						CLAY({ .id = CLAY_ID("OptionsScrollGutter"),
							.layout = {
								.layoutDirection = CLAY_TOP_TO_BOTTOM,
								.padding = { .left = 1, },
								.sizing = {
									.width = CLAY_SIZING_FIXED(SCROLLBAR_WIDTH),
									.height = CLAY_SIZING_GROW(0)
								},
							},
							.backgroundColor = ToClayColor(BACKGROUND_BLACK),
						})
						{
							rec scrollGutterDrawRec = GetClayElementDrawRec(CLAY_ID("OptionsScrollGutter"));
							v2 scrollBarSize = NewV2(
								SCROLLBAR_WIDTH - 2,
								optionsListDrawRec.Height * scrollbarSizePercent
							);
							r32 scrollBarOffsetY = ClampR32((scrollGutterDrawRec.Height - scrollBarSize.Height) * scrollbarYPercent, 0.0f, scrollGutterDrawRec.Height);
							CLAY({ .id = CLAY_ID("OptionsScrollBar"),
								.floating = {
									.attachTo = CLAY_ATTACH_TO_PARENT,
									.offset = { .x = 1, .y = scrollBarOffsetY },
								},
								.layout = {
									.sizing = {
										.width = CLAY_SIZING_FIXED(scrollBarSize.X),
										.height = CLAY_SIZING_FIXED(scrollBarSize.Y),
									},
								},
								.backgroundColor = ToClayColor(OUTLINE_GRAY),
								.cornerRadius = CLAY_CORNER_RADIUS(scrollBarSize.Width/2),
							}) {}
						}
					}
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
