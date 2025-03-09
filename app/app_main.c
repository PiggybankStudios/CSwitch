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

#if 0
#include "Commctrl.h"
#endif

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
#include "app_tooltips.c"
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
	
	FontCharRange fontCharRanges[] = {
		FontCharRange_ASCII,
		FontCharRange_LatinExt,
		NewFontCharRangeSingle(UNICODE_ELLIPSIS_CODEPOINT),
		NewFontCharRangeSingle(UNICODE_RIGHT_ARROW_CODEPOINT),
	};
	
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
	
	InitTooltipState(stdHeap, &app->tooltip);
	InitVarArray(TooltipRegion, &app->tooltipRegions, stdHeap);
	app->nextTooltipId = 1;
	
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
	
	#if 0
	INITCOMMONCONTROLSEX commonControls = ZEROED;
	commonControls.dwSize = sizeof(commonControls);
	commonControls.dwICC = ICC_WIN95_CLASSES;
	BOOL initResult = InitCommonControlsEx(&commonControls);
	Assert(initResult == TRUE);
	app->tooltipWindowHandle = NULL;
	#endif
	
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
	
	UpdateTooltipState(&app->tooltipRegions, &app->tooltip);
	
	bool refreshScreen = false;
	if (app->isFileOpen && AppCheckForFileChanges()) { refreshScreen = true; }
	if (app->tooltip.isOpen) { refreshScreen = true; }
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
	for (uxx keyIndex = 0; keyIndex < Key_Count; keyIndex++) { if (IsKeyboardKeyDown(&appIn->keyboard, (Key)keyIndex) || IsKeyboardKeyReleased(&appIn->keyboard, (Key)keyIndex)) { refreshScreen = true; break; } }
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
	// v2i mousePosi = RoundV2i(appIn->mouse.position);
	v2 mousePos = appIn->mouse.position;
	
	if (appIn->droppedFilePaths.length > 0)
	{
		if (appIn->droppedFilePaths.length > 1) { PrintLine_W("WARNING: Dropped %llu files at the same time. We only support opening 1 file at a time!", (u64)appIn->droppedFilePaths.length); }
		Str8 droppedFilePath = *VarArrayGetFirst(Str8, &appIn->droppedFilePaths);
		PrintLine_I("Dropped file: \"%.*s\"", StrPrint(droppedFilePath));
		AppOpenFile(droppedFilePath);
	}
	
	#if 0
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_T))
	{
		HWND windowHandle = (HWND)platform->GetNativeWindowHandle();
		if (app->tooltipWindowHandle == NULL)
		{
			WriteLine_O("Opening tooltip...");
			POINT mouseScreenPos = { mousePosi.X, mousePosi.Y };
		    ClientToScreen(windowHandle, &mouseScreenPos);
		    PrintLine_D("Mouse Global Coordinates: (%d, %d)", mouseScreenPos.x, mouseScreenPos.y);
		    
			app->tooltipWindowHandle = CreateWindowEx(
				WS_EX_TOPMOST, TOOLTIPS_CLASS,
				NULL,
				WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				windowHandle,
				NULL, NULL, NULL
			);
			Assert(app->tooltipWindowHandle != NULL);
			ClearStruct(app->tooltipInfo);
			app->tooltipInfo.cbSize = sizeof(TOOLINFO);
			app->tooltipInfo.uFlags = TTF_TRACK | TTF_ABSOLUTE;
			app->tooltipInfo.hwnd = windowHandle;
			app->tooltipInfo.uId = 1;
			app->tooltipInfo.lParam = 1;
			app->tooltipInfo.hinst = NULL;
			app->tooltipInfo.lpszText = (LPSTR)AllocAndCopyCharsNt(stdHeap, "Hello Windows!", true);
			// app->tooltipInfo.rect = (RECT){ .left = mouseScreenPos.x, .top = mouseScreenPos.y, .right = mouseScreenPos.x+1, .bottom = mouseScreenPos.y+1 };
			// app->tooltipInfo.rect = (RECT){ 0, 0, 0, 0 };
		    
			LRESULT addToolResult = SendMessageA(app->tooltipWindowHandle, TTM_ADDTOOL, 0, (LPARAM)&app->tooltipInfo);
			if (addToolResult != TRUE) { DWORD errorCode = GetLastError(); PrintLine_D("SendMessage(TTM_ADDTOOL) failed: %s %d", Win32_GetErrorCodeStr(errorCode), errorCode); }
			// SendMessage(app->tooltipWindowHandle, TTM_SETTITLE, 0, 0);
			// SendMessage(app->tooltipWindowHandle, TTM_SETMAXTIPWIDTH, 0, 400);
			SendMessage(app->tooltipWindowHandle, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(mouseScreenPos.x, mouseScreenPos.y+20));
			SendMessage(app->tooltipWindowHandle, TTM_TRACKACTIVATE, TRUE, (LPARAM)&app->tooltipInfo);
			// SendMessage(app->tooltipWindowHandle, TTM_GETDELAYTIME, TTDT_AUTOPOP, 0);
			// SendMessage(app->tooltipWindowHandle, TTM_POP, 0, 0);
		}
		else
		{
			WriteLine_O("Closing tooltip");
			DestroyWindow(app->tooltipWindowHandle);
			app->tooltipWindowHandle = NULL;
		}
	}
	#endif
	
	// +========================================+
	// | Handle Ctrl+W and Ctrl+Shift+W Hotkeys |
	// +========================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_W) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		if (IsKeyboardKeyDown(&appIn->keyboard, Key_Shift))
		{
			shouldContinueRunning = false;
		}
		else
		{
			//TODO: Close the current tab once we have support for tabs
			AppCloseFile();
		}
	}
	
	// +==============================+
	// |     Handle Ctrl+O Hotkey     |
	// +==============================+
	bool shouldOpenFile = false;
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_O) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		shouldOpenFile = true;
	}
	
	// +==============================+
	// |     Handle Ctrl+E Hotkey     |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_E) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		if (app->recentFiles.length > 0)
		{
			RecentFile* mostRecentFile = VarArrayGetLast(RecentFile, &app->recentFiles);
			AppOpenFile(mostRecentFile->path);
		}
	}
	
	// +==============================+
	// |     Handle Ctrl+T Hotkey     |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_T) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		platform->SetWindowTopmost(!appIn->isWindowTopmost);
	}
	
	// +==============================+
	// |     Handle Tilde Hotkey      |
	// +==============================+
	#if DEBUG_BUILD
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Tilde))
	{
		Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
	}
	#endif
	
	// +==============================+
	// |          Rendering           |
	// +==============================+
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
			u16 fullscreenBorderThickness = (appIn->isWindowTopmost ? 1 : 0);
			CLAY({ .id = CLAY_ID("FullscreenContainer"),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
					.padding = CLAY_PADDING_ALL(fullscreenBorderThickness)
				},
				.border = {
					.color=ToClayColor(SELECTED_BLUE),
					.width=CLAY_BORDER_OUTSIDE(fullscreenBorderThickness),
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
							shouldOpenFile = true;
							app->isFileMenuOpen = false;
						} Clay__CloseElement();
						
						if (app->recentFiles.length > 0)
						{
							if (ClayTopSubmenu("Open Recent " UNICODE_RIGHT_ARROW_STR, app->isFileMenuOpen, &app->isOpenRecentSubmenuOpen, OPEN_RECENT_DROPDOWN_WIDTH, &app->icons[AppIcon_OpenRecent]))
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
							if (ClayBtn("Open Recent " UNICODE_RIGHT_ARROW_STR, false, &app->icons[AppIcon_OpenRecent])) { } Clay__CloseElement();
						}
						
						if (ClayBtn("Close File", app->isFileOpen, &app->icons[AppIcon_CloseFile]))
						{
							AppCloseFile();
							app->isFileMenuOpen = false;
						} Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					if (ClayTopBtn("View", &app->isWindowMenuOpen, false))
					{
						if (ClayBtnStr(ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "Disable" : "Enable"), TARGET_IS_WINDOWS, &app->icons[appIn->isWindowTopmost ? AppIcon_TopmostEnabled : AppIcon_TopmostDisabled]))
						{
							platform->SetWindowTopmost(!appIn->isWindowTopmost);
						} Clay__CloseElement();
						
						if (ClayBtnStr(ScratchPrintStr("Clip Names on %s", app->clipNamesOnLeftSide ? "Left" : "Right"), true, &app->icons[app->clipNamesOnLeftSide ? AppIcon_ClipLeft : AppIcon_ClipRight]))
						{
							app->clipNamesOnLeftSide = !app->clipNamesOnLeftSide;
						} Clay__CloseElement();
						
						#if DEBUG_BUILD
						if (ClayBtn(ScratchPrint("%s Clay UI Debug", Clay_IsDebugModeEnabled() ? "Hide" : "Show"), true, &app->icons[AppIcon_Debug]))
						{
							Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
						} Clay__CloseElement();
						#endif
						
						// if (ClayBtn("Close Window", true, &app->icons[AppIcon_CloseWindow]))
						// {
						// 	shouldContinueRunning = false;
						// 	app->isFileMenuOpen = false;
						// } Clay__CloseElement();
						
						Clay__CloseElement();
						Clay__CloseElement();
					} Clay__CloseElement();
					
					
					CLAY({ .layout = { .sizing = { .width=CLAY_SIZING_GROW(0) } } }) {}
					
					if (app->isFileOpen)
					{
						CLAY({ .id = CLAY_ID("FilePathDisplay") })
						{
							CLAY_TEXT(
								ToClayString(app->filePath),
								CLAY_TEXT_CONFIG({
									.fontId = app->clayUiFontId,
									.fontSize = UI_FONT_SIZE,
									.textColor = ToClayColor(TEXT_LIGHT_GRAY),
									.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
									.wrapMode = CLAY_TEXT_WRAP_NONE,
									.userData = { .contraction = TextContraction_EllipseFilePath },
								})
							);
						}
						CLAY({ .layout={ .sizing={ .width=CLAY_SIZING_FIXED(4) } } }) {}
					}
					
					#if DEBUG_BUILD
					//NOTE: This little visual makes it easier to tell when we are rendering new frames and when we are asleep by having a little bar move every frame
					CLAY({ .id=CLAY_ID("FrameUpdateIndicatorContainer"),
						.layout = {
							.sizing={ .width=CLAY_SIZING_FIXED(4), .height=CLAY_SIZING_GROW(0) },
							.layoutDirection = CLAY_TOP_TO_BOTTOM,
						},
						.backgroundColor = ToClayColor(Black),
					})
					{
						for (uxx pIndex = 0; pIndex < 10; pIndex++)
						{
							CLAY({
								.layout = { .sizing = { .width=CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(0.1f) } },
								.backgroundColor = ToClayColor(((appIn->frameIndex % 10) == pIndex) ? MonokaiWhite : Black),
							}) {}
						}
					}
					#endif
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
		
		// +==============================+
		// |    Update TooltipRegions     |
		// +==============================+
		if (app->filePathTooltipId != 0)
		{
			TooltipRegion* region = FindTooltipRegionById(&app->tooltipRegions, app->filePathTooltipId);
			if (region != nullptr)
			{
				rec filePathDisplayRec = GetClayElementDrawRec(CLAY_ID("FilePathDisplay"));
				region->mainRec = InflateRecY(filePathDisplayRec, 8);
			}
		}
		VarArrayLoop(&app->fileOptions, oIndex)
		{
			VarArrayLoopGet(FileOption, option, &app->fileOptions, oIndex);
			if (option->tooltipId != 0)
			{
				TooltipRegion* tooltip = FindTooltipRegionById(&app->tooltipRegions, option->tooltipId);
				NotNull(tooltip);
				tooltip->enabled = false;
				Str8 buttonUiId = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(option->name));
				rec buttonDrawRec = GetClayElementDrawRec(ToClayId(buttonUiId));
				rec scrollDrawRec = GetClayElementDrawRec(CLAY_ID("OptionsList"));
				if (scrollDrawRec.Width > 0 && scrollDrawRec.Height > 0 &&
					buttonDrawRec.Width > 0 && buttonDrawRec.Height > 0)
				{
					buttonDrawRec = OverlapPartRec(buttonDrawRec, scrollDrawRec);
					if (buttonDrawRec.Width > 0 && buttonDrawRec.Height > 0)
					{
						tooltip->enabled = true;
						tooltip->mainRec = buttonDrawRec;
					}
				}
			}
		}
		
		RenderTooltip(&app->tooltip);
		
		#if 0
		FontAtlas* fontAtlas = GetFontAtlas(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE);
		BindFontEx(&app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE);
		rec textRec = NewRec(50, screenSize.Height/2, mousePos.X - 50, fontAtlas->lineHeight);
		if (textRec.Width < 5) { textRec.Width = 5; }
		DrawRectangleOutline(textRec, 2, MonokaiRed);
		Str8 shortenedPath = ShortenFilePathToFitWidth(scratch, &app->uiFont, UI_FONT_SIZE, UI_FONT_STYLE, app->filePath, textRec.Width, StrLit("..."));
		DrawText(shortenedPath, NewV2(textRec.X, textRec.Y + textRec.Height/2 + fontAtlas->centerOffset), MonokaiWhite);
		#endif
	}
	EndFrame();
	
	// +==============================+
	// |   Handle Open File Dialog    |
	// +==============================+
	if (shouldOpenFile)
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
	}
	
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
