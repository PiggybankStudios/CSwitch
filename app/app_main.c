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
#include "file_fmt/file_fmt_all.h"
#include "gfx/gfx_all.h"
#include "gfx/gfx_system_global.h"
#include "input/input_all.h"
#include "lib/lib_all.h"
#include "mem/mem_all.h"
#include "misc/misc_all.h"
#include "os/os_all.h"
#include "parse/parse_all.h"
#include "phys/phys_all.h"
#include "struct/struct_all.h"
#include "ui/ui_all.h"

#if 0
#include "Commctrl.h"
#endif

// +--------------------------------------------------------------+
// |                         Header Files                         |
// +--------------------------------------------------------------+
#include "platform_interface.h"
#include "app_theme_funcs.h"
#include "app_theme.h"
#include "app_icons.h"
#include "app_settings.h"
#include "app_resources.h"
#include "app_main.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
static AppData* app = nullptr;
static AppInput* appIn = nullptr;
static Arena* uiArena = nullptr;

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
#include "app_theme_funcs.c"
#include "app_theme.c"
#include "app_theme_funcs_implementations.c"
#include "app_clay_helpers.c"
#include "app_textbox.c"
#include "app_popup_dialog.c"
#include "app_helpers.c"
#include "app_tab.c"
#include "app_clay.c"

// +==============================+
// |         TestWorkItem         |
// +==============================+
// Result TestWorkItem(ThreadPoolThread* thread, plex ThreadPoolWorkItem* workItem)
THREAD_POOL_WORK_ITEM_FUNC_DEF(TestWorkItem)
{
	uxx numIterations = workItem->subject.id0;
	// Str8* allocatedStr = *GetStructInWorkSubject(Str8, &workItem->subject, 0);
	Str8 allocatedStr = workItem->subject.string0;
	Str8 threadName = GetStandardPeopleFirstName(thread->id);
	PrintLine_D("%.*s STARTED by %.*s...", StrPrint(allocatedStr), StrPrint(threadName));
	for (uxx i = 0; i < numIterations; i++)
	{
		if (thread->stopRequested) { return Result_Stopped; }
		PrintLine_D("%.*s worked (%llu/%llu) by %.*s...", StrPrint(allocatedStr), i+1, numIterations, StrPrint(threadName));
		OsSleepMs(1000);
	}
	PrintLine_D("%.*s ENDED by %.*s...", StrPrint(allocatedStr), StrPrint(threadName));
	return Result_Success;
}

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
	
	#if NOTIFICATION_QUEUE_AVAILABLE
	SetGlobalNotificationQueue((app != nullptr) ? &app->notificationQueue : nullptr);
	#endif
}

#if 0
#define NO_KEY_IMAGE_SHEET_FRAME_X 7
#define NO_KEY_IMAGE_SHEET_FRAME_Y 6
#define NO_KEY_IMAGE_SHEET_FRAME MakeV2i(NO_KEY_IMAGE_SHEET_FRAME_X, NO_KEY_IMAGE_SHEET_FRAME_Y)
v2i GetSheetFrameForKey(Key key, bool isPressed, i32* imageWidth, bool* isWideKey)
{
	i32 yOffset = (isPressed ? 7 : 0);
	i32 yOffsetWide = (isPressed ? 4 : 0);
	SetOptionalOutPntr(isWideKey, false);
	SetOptionalOutPntr(imageWidth, 13);
	switch (key)
	{
		case Key_Up:           return MakeV2i(0, 0 + yOffset);
		case Key_Down:         return MakeV2i(1, 0 + yOffset);
		case Key_Left:         return MakeV2i(2, 0 + yOffset);
		case Key_Right:        return MakeV2i(3, 0 + yOffset);
		case Key_F1:           return MakeV2i(4, 0 + yOffset);
		case Key_F2:           return MakeV2i(5, 0 + yOffset);
		case Key_F3:           return MakeV2i(6, 0 + yOffset);
		case Key_F4:           return MakeV2i(7, 0 + yOffset);
		
		case Key_F5:           return MakeV2i(0, 1 + yOffset);
		case Key_F6:           return MakeV2i(1, 1 + yOffset);
		case Key_F7:           return MakeV2i(2, 1 + yOffset);
		case Key_F8:           return MakeV2i(3, 1 + yOffset);
		case Key_F9:           return MakeV2i(4, 1 + yOffset);
		case Key_F10:          return MakeV2i(5, 1 + yOffset);
		case Key_F11:          return MakeV2i(6, 1 + yOffset);
		case Key_F12:          return MakeV2i(7, 1 + yOffset);
		
		case Key_A:            return MakeV2i(0, 2 + yOffset);
		case Key_B:            return MakeV2i(1, 2 + yOffset);
		case Key_C:            return MakeV2i(2, 2 + yOffset);
		case Key_D:            return MakeV2i(3, 2 + yOffset);
		case Key_E:            return MakeV2i(4, 2 + yOffset);
		case Key_F:            return MakeV2i(5, 2 + yOffset);
		case Key_G:            return MakeV2i(6, 2 + yOffset);
		case Key_H:            return MakeV2i(7, 2 + yOffset);
		
		case Key_I:            return MakeV2i(0, 3 + yOffset);
		case Key_J:            return MakeV2i(1, 3 + yOffset);
		case Key_K:            return MakeV2i(2, 3 + yOffset);
		case Key_L:            return MakeV2i(3, 3 + yOffset);
		case Key_M:            return MakeV2i(4, 3 + yOffset);
		case Key_N:            return MakeV2i(5, 3 + yOffset);
		case Key_O:            return MakeV2i(6, 3 + yOffset);
		case Key_P:            return MakeV2i(7, 3 + yOffset);
		
		case Key_Q:            return MakeV2i(0, 4 + yOffset);
		case Key_R:            return MakeV2i(1, 4 + yOffset);
		case Key_S:            return MakeV2i(2, 4 + yOffset);
		case Key_T:            return MakeV2i(3, 4 + yOffset);
		case Key_U:            return MakeV2i(4, 4 + yOffset);
		case Key_V:            return MakeV2i(5, 4 + yOffset);
		case Key_W:            return MakeV2i(6, 4 + yOffset);
		case Key_X:            return MakeV2i(7, 4 + yOffset);
		
		case Key_Y:            return MakeV2i(0, 5 + yOffset);
		case Key_Z:            return MakeV2i(1, 5 + yOffset);
		case Key_Period:       return MakeV2i(2, 5 + yOffset);
		case Key_Comma:        return MakeV2i(3, 5 + yOffset);
		// case Key_Question:  return MakeV2i(4, 5 + yOffset);
		case Key_ForwardSlash: return MakeV2i(5, 5 + yOffset);
		// case Key_BackSlash: return MakeV2i(6, 5 + yOffset);
		case Key_Pipe:         return MakeV2i(7, 5 + yOffset);
		
		case Key_Quote:        return MakeV2i(0, 6 + yOffset);
		case Key_OpenBracket:  return MakeV2i(1, 6 + yOffset);
		case Key_CloseBracket: return MakeV2i(2, 6 + yOffset);
		case Key_Plus:         return MakeV2i(3, 6 + yOffset);
		case Key_Minus:        return MakeV2i(4, 6 + yOffset);
		case Key_Tilde:        return MakeV2i(5, 6 + yOffset);
		// case Key_None:      return MakeV2i(6, 6 + yOffset);
		// case Key_None:      return MakeV2i(7, 6 + yOffset);
		
		case Key_Tab:         SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 24); return MakeV2i(0, 0 + yOffsetWide);
		case Key_Escape:      SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(1, 0 + yOffsetWide);
		case Key_PrintScreen: SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 28); return MakeV2i(2, 0 + yOffsetWide);
		case Key_Backspace:   SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 28); return MakeV2i(3, 0 + yOffsetWide);
		
		case Key_Shift:       SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 28); return MakeV2i(0, 1 + yOffsetWide);
		case Key_PageUp:      SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 20); return MakeV2i(1, 1 + yOffsetWide);
		case Key_PageDown:    SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 20); return MakeV2i(2, 1 + yOffsetWide);
		case Key_Enter:       SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 28); return MakeV2i(3, 1 + yOffsetWide);
		
		case Key_Control:     SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 24); return MakeV2i(0, 2 + yOffsetWide);
		case Key_Alt:         SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(1, 2 + yOffsetWide);
		case Key_Space:       SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 30); return MakeV2i(2, 2 + yOffsetWide);
		case Key_Insert:      SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(3, 2 + yOffsetWide);
		
		case Key_Delete:      SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(0, 3 + yOffsetWide);
		case Key_End:         SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(1, 3 + yOffsetWide);
		case Key_Home:        SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 18); return MakeV2i(2, 3 + yOffsetWide);
		case Key_Pause:       SetOptionalOutPntr(isWideKey, true); SetOptionalOutPntr(imageWidth, 28); return MakeV2i(3, 3 + yOffsetWide);
		
		default: return MakeV2i(NO_KEY_IMAGE_SHEET_FRAME_X, NO_KEY_IMAGE_SHEET_FRAME_Y + yOffset);
	}
}
#endif

// +==============================+
// |           AppInit            |
// +==============================+
// void* AppInit(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi)
EXPORT_FUNC APP_INIT_DEF(AppInit)
{
	TracyCZoneN(_funcZone, "AppInit", true);
	#if !BUILD_INTO_SINGLE_UNIT
	InitScratchArenasVirtual(Gigabytes(4));
	#endif
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	
	AppData* appData = AllocType(AppData, inPlatformInfo->platformStdHeap);
	ClearPointer(appData);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, (void*)appData, nullptr);
	
	#if THREAD_POOL_TEST
	InitThreadPool(stdHeap, StrLit("TestThreadPool"), true, true, Gigabytes(4), &app->threadPool);
	AddThreadToPool(&app->threadPool);
	AddThreadToPool(&app->threadPool);
	AddThreadToPool(&app->threadPool);
	AddThreadToPool(&app->threadPool);
	#endif
	
	InitNotificationQueue(stdHeap, &app->notificationQueue);
	
	InitAppSettings(stdHeap, &app->settings);
	LoadAppSettings();
	
	InitAppResources(&app->resources);
	LoadNotificationIcons();
	
	platform->SetWindowTitle(StrLit(PROJECT_READABLE_NAME_STR));
	LoadWindowIcon();
	
	for (uxx iIndex = 1; iIndex < AppIcon_Count; iIndex++)
	{
		AppIcon iconEnum = (AppIcon)iIndex;
		const char* iconPath = GetAppIconPath(iconEnum);
		if (iconPath != nullptr)
		{
			ImageData iconImageData = LoadImageData(scratch, iconPath);
			app->icons[iIndex] = InitTexture(stdHeap, MakeStr8Nt(GetAppIconStr(iconEnum)), iconImageData.size, iconImageData.pixels, TextureFlag_NoMipmaps);
			Assert(app->icons[iIndex].error == Result_Success);
		}
	}
	
	InitRandomSeriesDefault(&app->random);
	SeedRandomSeriesU64(&app->random, OsGetCurrentTimestamp(false));
	
	InitPerfGraph(&app->perfGraph, 1000.0f/60.0f); //TODO: How do we know the target framerate?
	
	InitCompiledShader(&app->mainShader, stdHeap, main2d);
	
	app->uiFontSize = DEFAULT_UI_FONT_SIZE;
	app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
	app->uiScale = 1.0f;
	bool fontBakeSuccess = AppCreateFonts();
	Assert(fontBakeSuccess);
	
	InitClayUIRenderer(stdHeap, V2_Zero, &app->clay);
	AttachTooltipRegistryToUIRenderer(&app->clay, &app->tooltips);
	app->clayUiFontId = AddClayUIRendererFont(&app->clay, &app->uiFont, UI_FONT_STYLE);
	app->clayMainFontId = AddClayUIRendererFont(&app->clay, &app->mainFont, MAIN_FONT_STYLE);
	
	app->usingKeyboardToSelect = false;
	
	InitTooltipRegistry(stdHeap, &app->tooltips);
	
	// InitClayTextbox(stdHeap, StrLit("TestTextbox"), StrLit("Hello Text!"), app->clayMainFontId, app->mainFontSize, &app->testTextbox);
	
	#if DEBUG_BUILD
	app->enableFrameUpdateIndicator = false;
	#endif
	app->minimalModeEnabled = false;
	
	InitFileWatches(&app->fileWatches);
	InitVarArray(FileTab, &app->tabs, stdHeap);
	
	InitThemeDefFuncArgInfos();
	AppTryLoadDefaultTheme(true);
	AppLoadUserTheme();
	if (!TryParseThemeMode(app->settings.themeMode, &app->currentThemeMode))
	{
		NotifyPrint_W("Unknown theme mode in settings file: \"%.*s\"\nDefaulting to Dark Mode", StrPrint(app->settings.themeMode));
		app->currentThemeMode = ThemeMode_Dark;
		SetAppSettingStr8Pntr(&app->settings, &app->settings.themeMode, MakeStr8Nt(GetThemeModeStr(app->currentThemeMode)));
		SaveAppSettings();
	}
	AppBakeTheme(true);
	
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
			FileTab* newTab = AppOpenFileTab(pathArgument);
			if (newTab != nullptr) { break; }
		}
		else
		{
			NotifyPrint_E("Command line path does not point to a file: \"%.*s\"", StrPrint(pathArgument));
		}
		
		argIndex++;
		pathArgument = GetNamelessProgramArg(platformInfo->programArgs, argIndex);
	}
	
	if (!wasCmdPathGiven && app->recentFiles.length > 0)
	{
		RecentFile* mostRecentFile = VarArrayGetLast(RecentFile, &app->recentFiles);
		AppOpenFileTab(mostRecentFile->path);
	}
	
	#if 0
	INITCOMMONCONTROLSEX commonControls = ZEROED;
	commonControls.dwSize = sizeof(commonControls);
	commonControls.dwICC = ICC_WIN95_CLASSES;
	BOOL initResult = InitCommonControlsEx(&commonControls);
	Assert(initResult == TRUE);
	app->tooltipWindowHandle = NULL;
	#endif
	
	app->testSheet = LoadSpriteSheet(stdHeap, StrLit("testSheet"), FilePathLit("resources/image/notifications_2x2.png"), true);
	if (app->testSheet.error == Result_Success)
	{
		PrintLine_E("Loaded testSheet: grid=%dx%d cell=%dx%d texture=%dx%d",
			app->testSheet.gridWidth, app->testSheet.gridHeight,
			app->testSheet.cellWidth, app->testSheet.cellHeight,
			app->testSheet.texture.Width, app->testSheet.texture.Height
		);
	}
	else
	{
		PrintLine_E("Failed to parse SpriteSheet: %s", GetResultStr(app->testSheet.error));
	}
	
	app->initialized = true;
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
	TracyCZoneEnd(_funcZone);
	return (void*)app;
}

// +==============================+
// |       AppBeforeReload        |
// +==============================+
// bool AppBeforeReload(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr)
EXPORT_FUNC APP_BEFORE_RELOAD_DEF(AppBeforeReload)
{
	bool shouldReload = true;
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, nullptr);
	
	WriteLine_W("App is preparing for DLL reload...");
	//TODO: Anything that needs to be saved before the DLL reload should be done here
	
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
	return shouldReload;
}

// +==============================+
// |        AppAfterReload        |
// +==============================+
// void AppAfterReload(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr)
EXPORT_FUNC APP_AFTER_RELOAD_DEF(AppAfterReload)
{
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, nullptr);
	
	WriteLine_I("New app DLL was loaded!");
	app->shouldRenderAfterReload = true;
	InitThemeDefFuncArgInfos();
	
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
}

// +==============================+
// |          AppUpdate           |
// +==============================+
// bool AppUpdate(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr, AppInput* appInput)
EXPORT_FUNC APP_UPDATE_DEF(AppUpdate)
{
	OsTime beforeUpdateTime = OsGetTime();
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, appInput);
	TracyCZoneN(_funcZone, "AppUpdate", true);
	
	TracyCZoneN(Zone_Update, "Update", true);
	app->notificationQueue.currentProgramTime = appIn->programTime;
	UpdateFileWatches(&app->fileWatches);
	UpdatePopupDialog(&app->popup);
	if (appIn->frameIndex != 0 && app->renderedLastFrame)
	{
		r32 fullUpdateMs = platformInfo->updateMs + app->prevUpdateMs;
		UpdatePerfGraph(&app->perfGraph, fullUpdateMs, ((r32)appIn->unclampedElapsedMsR64 - fullUpdateMs));
	}
	
	bool refreshScreen = (app->sleepingDisabled || app->showPerfGraph);
	
	// +==============================+
	// |  Handle Finished WorkItems   |
	// +==============================+
	#if THREAD_POOL_TEST
	ThreadPoolWorkItem* finishedWorkItem = nullptr;
	while ((finishedWorkItem = GetFinishedThreadPoolWorkItem(&app->threadPool)) != nullptr)
	{
		PrintLine_O("%.*s FINISHED: %s", StrPrint(finishedWorkItem->subject.string0), GetResultStr(finishedWorkItem->result));
		FreeThreadPoolWorkItem(&app->threadPool, finishedWorkItem);
	}
	#endif
	
	// +==============================+
	// |     Handle Dropped Files     |
	// +==============================+
	VarArrayLoop(&appIn->droppedFilePaths, pIndex)
	{
		VarArrayLoopGet(Str8, droppedFilePath, &appIn->droppedFilePaths, pIndex);
		PrintLine_I("Dropped file: \"%.*s\"", StrPrint(*droppedFilePath));
		AppOpenFileTab(*droppedFilePath);
		refreshScreen = true;
	}
	
	// +====================================+
	// | Determine if Screen Needs Refresh  |
	// +====================================+
	{
		if (AppCheckForFileChanges()) { refreshScreen = true; }
		if (app->wasClayScrollingPrevFrame) { refreshScreen = true; }
		if (app->tooltips.openTooltipId != TOOLTIP_ID_INVALID || app->tooltips.hoverTooltipId != TOOLTIP_ID_INVALID) { refreshScreen = true; }
		if (app->recentFilesWatchId != 0 && HasFileWatchChangedWithDelay(&app->fileWatches, app->recentFilesWatchId, RECENT_FILES_RELOAD_DELAY))
		{
			ClearFileWatchChanged(&app->fileWatches, app->recentFilesWatchId);
			AppLoadRecentFilesList();
			refreshScreen = true;
		}
		#if !USE_BUNDLED_RESOURCES
		if (app->defaultThemeFileWatchId != 0 && HasFileWatchChangedWithDelay(&app->fileWatches, app->defaultThemeFileWatchId, DEFAULT_THEME_RELOAD_DELAY))
		{
			ClearFileWatchChanged(&app->fileWatches, app->defaultThemeFileWatchId);
			WriteLine_D("Default theme file write time changed. Auto-reloading!");
			if (AppTryLoadDefaultTheme(false)) { Notify_D("Reloaded default theme"); }
			else { Notify_W("Failed to reload default theme!"); }
			AppBakeTheme(false);
			
			refreshScreen = true;
		}
		#endif // !USE_BUNDLED_RESOURCES
		if (app->userThemeFileWatchId != 0 && HasFileWatchChangedWithDelay(&app->fileWatches, app->userThemeFileWatchId, USER_THEME_RELOAD_DELAY))
		{
			ClearFileWatchChanged(&app->fileWatches, app->userThemeFileWatchId);
			WriteLine_D("User theme file write time changed. Auto-reloading!");
			AppLoadUserTheme();
			AppBakeTheme(false);
			refreshScreen = true;
		}
		if (app->popup.isOpen && TimeSinceBy(appIn->programTime, app->popup.openTime) <= POPUP_OPEN_ANIM_TIME) { refreshScreen = true; }
		else if (!app->popup.isOpen && app->popup.isVisible && TimeSinceBy(appIn->programTime, app->popup.closeTime) <= POPUP_CLOSE_ANIM_TIME) { refreshScreen = true; }
		if (app->notificationQueue.notifications.length > 0) { refreshScreen = true; }
		if (!AreEqual(appIn->mouse.prevPosition, appIn->mouse.position) && (appIn->mouse.isOverWindow || appIn->mouse.wasOverWindow)) { refreshScreen = true; }
		if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Left) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Left)) { refreshScreen = true; }
		if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Right) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Right)) { refreshScreen = true; }
		if (IsMouseBtnReleased(&appIn->mouse, MouseBtn_Middle) || IsMouseBtnDown(&appIn->mouse, MouseBtn_Middle)) { refreshScreen = true; }
		for (uxx keyIndex = 0; keyIndex < Key_Count; keyIndex++) { if (IsKeyboardKeyDown(&appIn->keyboard, (Key)keyIndex) || IsKeyboardKeyReleased(&appIn->keyboard, (Key)keyIndex)) { refreshScreen = true; break; } }
		if (appIn->isFullscreenChanged || appIn->isMinimizedChanged || appIn->isFocusedChanged || appIn->screenSizeChanged) { refreshScreen = true; }
		if (appIn->mouse.scrollDelta.X != 0 || appIn->mouse.scrollDelta.Y != 0) { refreshScreen = true; }
		if (app->shouldRenderAfterReload) { refreshScreen = true; app->shouldRenderAfterReload = false; }
		
		if (refreshScreen) { app->numFramesConsecutivelyRendered = 0; }
		else { app->numFramesConsecutivelyRendered++; }
		
		if (!refreshScreen && app->numFramesConsecutivelyRendered >= NUM_FRAMES_BEFORE_SLEEP)
		{
			ScratchEnd(scratch);
			ScratchEnd(scratch2);
			ScratchEnd(scratch3);
			app->renderedLastFrame = false;
			TracyCZoneEnd(Zone_Update);
			TracyCZoneEnd(_funcZone);
			return false;
		}
	}
	
	v2i screenSizei = appIn->screenSize;
	v2 screenSize = ToV2Fromi(appIn->screenSize);
	// v2 screenCenter = Div(screenSize, 2.0f);
	// v2i mousePosi = RoundV2i(appIn->mouse.position);
	v2 mousePos = appIn->mouse.position;
	FontNewFrame(&app->uiFont, appIn->programTime);
	FontNewFrame(&app->mainFont, appIn->programTime);
	UpdateTooltipRegistry(&app->tooltips);
	
	// +======================================+
	// | Calculate Longest Abbreviation Width |
	// +======================================+
	VarArrayLoop(&app->tabs, tIndex)
	{
		VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
		if (tab->longestAbbreviationWidthFontSize != app->mainFontSize)
		{
			CalculateLongestAbbreviationWidth(tab);
			DebugAssert(tab->longestAbbreviationWidthFontSize == app->mainFontSize);
		}
	}
	
	// +==============================+
	// | Native Windows Tooltip Test  |
	// +==============================+
	#if 0
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_T, false))
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
	
	// +==============================+
	// |       Thread Pool Test       |
	// +==============================+
	#if THREAD_POOL_TEST
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_W, true))
	{
		WorkSubject subject = ZEROED;
		subject.id0 = GetRandU32Range(&app->random, 3, 16);
		// Str8* allocatedStr = AllocStructInWorkSubject(Str8, stdHeap, &subject, 0);
		// NotNull(allocatedStr);
		// allocatedStr->length = GetRandU32Range(&app->random, 2, 10);
		// allocatedStr->chars = AllocArray(char, stdHeap, allocatedStr->length);
		// NotNull(allocatedStr->chars);
		// for (uxx cIndex = 0; cIndex < allocatedStr->length; cIndex++)
		// {
		// 	allocatedStr->chars[cIndex] = (char)GetRandU32Range(&app->random, 'A', 'Z'+1);
		// }
		// Str8 allocatedStr = AllocStringInWorkSubject(stdHeap, &subject, 0, StrLit("Hello World"), false);
		// for (uxx cIndex = 0; cIndex < allocatedStr.length; cIndex++)
		// {
		// 	if (cIndex == 0 || GetRandU32Range(&app->random, 0, 100) < 20) { allocatedStr.chars[cIndex] = (char)GetRandU32Range(&app->random, 'A', 'Z'+1); }
		// 	else { allocatedStr.chars[cIndex] = (char)GetRandU32Range(&app->random, 'a', 'z'+1); }
		// }
		Str8 allocatedStr = AllocStringInWorkSubject(stdHeap, &subject, 0, MakeStr8Nt(GetStandardRockName(GetRandU32(&app->random))), false);
		ThreadPoolWorkItem* newItem = AddWorkItemToThreadPool(&app->threadPool, TestWorkItem, &subject);
		NotNull(newItem);
		PrintLine_D("Queued %.*s as item %llu", StrPrint(allocatedStr), newItem->id);
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_K, true))
	{
		bool stoppedAThread = false;
		for (uxx tIndex = 0; tIndex < app->threadPool.threads.length; tIndex++)
		{
			ThreadPoolThread* thread = BktArrayGet(ThreadPoolThread, &app->threadPool.threads, tIndex);
			if (!thread->stopRequested)
			{
				thread->stopRequested = true;
				stoppedAThread = true;
				break;
			}
		}
		if (!stoppedAThread)
		{
			FreeThreadPool(&app->threadPool);
		}
	}
	#endif
	
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_N, true))
	{
		DbgLevel level = (DbgLevel)GetRandU32Range(&app->random, 1, DbgLevel_Count);
		AddNotificationToQueue(&app->notificationQueue, level, ScratchPrintStr("%s notification is here!", GetDbgLevelStr(level)));
	}
	
	// +========================================+
	// | Handle Ctrl+W and Ctrl+Shift+W Hotkeys |
	// +========================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_W, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		if (IsKeyboardKeyDown(&appIn->keyboard, Key_Shift))
		{
			platform->RequestQuit();
		}
		else
		{
			if (app->currentTab != nullptr)
			{
				AppCloseFileTab(app->currentTabIndex);
			}
		}
	}
	
	// +==============================+
	// | F6 Toggles Performance Graph |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_F6, false))
	{
		app->showPerfGraph = !app->showPerfGraph;
	}
	
	// +==============================+
	// |       Handle F9 Hotkey       |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_F9, false))
	{
		ThemeMode otherThemeMode = ((DEBUG_BUILD && IsKeyboardKeyDown(&appIn->keyboard, Key_Shift))
			? ThemeMode_Debug
			: ((app->currentThemeMode == ThemeMode_Dark)
				? ThemeMode_Light
				: ThemeMode_Dark
			)
		);
		app->currentThemeMode = otherThemeMode;
		SetAppSettingStr8Pntr(&app->settings, &app->settings.themeMode, MakeStr8Nt(GetThemeModeStr(app->currentThemeMode)));
		SaveAppSettings();
		AppBakeTheme(false);
	}
	
	// +==============================+
	// |      Handle F10 Hotkey       |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_F10, false))
	{
		app->settings.smallButtons = !app->settings.smallButtons;
		SaveAppSettings();
	}
	
	// +==============================+
	// |      Handle F11 Hotkey       |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_F11, false))
	{
		app->minimalModeEnabled = !app->minimalModeEnabled;
	}
	
	// +==============================+
	// |      Handle Escape Key       |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Escape, false))
	{
		if (app->popup.isOpen)
		{
			ClosePopupDialog(&app->popup, nullptr);
		}
		else if (app->isFileMenuOpen)
		{
			app->isFileMenuOpen = false;
			app->isOpenRecentSubmenuOpen = false;
		}
		else if (app->isViewMenuOpen)
		{
			app->isViewMenuOpen = false;
		}
		else if (app->minimalModeEnabled)
		{
			app->minimalModeEnabled = false;
		}
	}
	
	// +==================================================+
	// | Handle Ctrl+Plus, Ctrl+Minus, and Ctrl+0 Hotkeys |
	// +==================================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Plus, true) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		AppChangeFontSize(true);
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Minus, true) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		AppChangeFontSize(false);
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_0, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		app->uiFontSize = DEFAULT_UI_FONT_SIZE;
		app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
		app->uiScale = 1.0f;
		bool fontBakeSuccess = AppCreateFonts();
		Assert(fontBakeSuccess);
	}
	
	// +==============================+
	// |   Handle Ctrl+ScrollWheel    |
	// +==============================+
	if (IsKeyboardKeyDown(&appIn->keyboard, Key_Control) && appIn->mouse.scrollDelta.Y != 0)
	{
		AppChangeFontSize(appIn->mouse.scrollDelta.Y > 0);
	}
	
	// +================================+
	// | Handle Alt+F and Alt+V Hotkeys |
	// +================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_F, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Alt))
	{
		app->isFileMenuOpen = !app->isFileMenuOpen;
		if (app->isFileMenuOpen)
		{
			app->isViewMenuOpen = false;
			app->keepFileMenuOpenUntilMouseOver = true;
		}
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_V, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Alt))
	{
		app->isViewMenuOpen = !app->isViewMenuOpen;
		if (app->isViewMenuOpen)
		{
			app->isFileMenuOpen = false;
			app->keepViewMenuOpenUntilMouseOver = true;
		}
	}
	
	// +==============================+
	// |     Handle Ctrl+O Hotkey     |
	// +==============================+
	bool shouldOpenFile = false;
	bool shouldOpenThemeFile = false;
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_O, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		shouldOpenFile = true;
	}
	
	// +========================================+
	// | Handle Ctrl+Tab/Ctrl+Shift+Tab Hotkeys |
	// +========================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Tab, true) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		if (app->tabs.length > 1)
		{
			if (IsKeyboardKeyDown(&appIn->keyboard, Key_Shift))
			{
				AppChangeTab(app->currentTabIndex > 0 ? app->currentTabIndex-1 : app->tabs.length-1);
			}
			else
			{
				AppChangeTab((app->currentTabIndex+1) % app->tabs.length);
			}
		}
	}
	
	// +==============================+
	// |     Handle Ctrl+E Hotkey     |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_E, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
		{
			VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex-1);
			if (recentFile->fileExists && AppFindTabForPath(recentFile->path) == nullptr)
			{
				if (AppOpenFileTab(recentFile->path) != nullptr) { break; }
			}
		}
	}
	
	// +==============================+
	// |     Handle Ctrl+T Hotkey     |
	// +==============================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_T, false) && IsKeyboardKeyDown(&appIn->keyboard, Key_Control))
	{
		platform->SetWindowTopmost(!appIn->isWindowTopmost);
	}
	
	// +==============================+
	// |     Handle Tilde Hotkey      |
	// +==============================+
	#if DEBUG_BUILD
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Tilde, false))
	{
		Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
	}
	#endif
	
	// +======================================+
	// | Handle Home/End and PageUp/PageDown  |
	// +======================================+
	//TODO: These should probably move the app->currentTab->selectedOptionIndex if app->usingKeyboardToSelect
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Home, false))
	{
		Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
		if (optionsListScrollData.found) { optionsListScrollData.scrollTarget->Y = 0; }
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_End, false))
	{
		Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
		if (optionsListScrollData.found)
		{
			r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
			optionsListScrollData.scrollTarget->Y = -maxScroll;
		}
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_PageUp, true))
	{
		Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
		if (optionsListScrollData.found)
		{
			r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
			optionsListScrollData.scrollTarget->Y = ClampR32(optionsListScrollData.scrollTarget->Y + optionsListScrollData.scrollContainerDimensions.Height, -maxScroll, 0);
		}
	}
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_PageDown, true))
	{
		Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
		if (optionsListScrollData.found)
		{
			r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
			optionsListScrollData.scrollTarget->Y = ClampR32(optionsListScrollData.scrollTarget->Y - optionsListScrollData.scrollContainerDimensions.Height, -maxScroll, 0);
		}
	}
	
	// +====================================+
	// | Handle Arrow Keys to Select Option |
	// +====================================+
	if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Left, true) ||
		IsKeyboardKeyPressed(&appIn->keyboard, Key_Right, true) ||
		IsKeyboardKeyPressed(&appIn->keyboard, Key_Up, true) ||
		IsKeyboardKeyPressed(&appIn->keyboard, Key_Down, true))
	{
		app->usingKeyboardToSelect = true;
		if (app->currentTab != nullptr)
		{
			bool movedSelection = false;
			
			if (app->currentTab->selectedOptionIndex == -1 && app->currentTab->fileOptions.length > 0)
			{
				//TODO: Could we somehow choose the option thats near the middle of the screen?
				app->currentTab->selectedOptionIndex = 0;
				movedSelection = true;
			}
			else if (app->currentTab->selectedOptionIndex >= 0)
			{
				if (app->settings.smallButtons)
				{
					r32 optionsAreaWidth = screenSize.Width - (app->minimalModeEnabled ? 0.0f : UI_R32(SCROLLBAR_WIDTH)) - (r32)(UI_U16(4) * 2);
					u16 buttonMargin = UI_U16(SMALL_BTN_MARGIN);
					r32 buttonWidth = app->currentTab->longestAbbreviationWidth + (r32)UI_U16(SMALL_BTN_PADDING_X)*2;
					i32 numColumns = FloorR32i((optionsAreaWidth - (r32)buttonMargin) / (buttonWidth + (r32)buttonMargin));
					if (numColumns <= 0) { numColumns = 1; }
					i32 numRows = CeilDivI32((i32)app->currentTab->fileOptions.length, numColumns);
					
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Up, true))
					{
						if (app->currentTab->selectedOptionIndex >= numColumns)
						{
							app->currentTab->selectedOptionIndex -= numColumns;
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Up, false))
						{
							app->currentTab->selectedOptionIndex = (ixx)(app->currentTab->fileOptions.length-1) - ((ixx)numColumns - app->currentTab->selectedOptionIndex);
							movedSelection = true;
						}
					}
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Down, true))
					{
						if (((i32)app->currentTab->selectedOptionIndex / numColumns) < numRows-1)
						{
							app->currentTab->selectedOptionIndex += numColumns;
							if ((uxx)app->currentTab->selectedOptionIndex >= app->currentTab->fileOptions.length)
							{
								app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							}
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Down, false))
						{
							app->currentTab->selectedOptionIndex = (app->currentTab->selectedOptionIndex + numColumns) % (ixx)app->currentTab->fileOptions.length;
							movedSelection = true;
						}
					}
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Left, true))
					{
						if (app->currentTab->selectedOptionIndex > 0)
						{
							app->currentTab->selectedOptionIndex--;
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Left, false))
						{
							app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							movedSelection = true;
						}
					}
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Right, true))
					{
						if ((uxx)app->currentTab->selectedOptionIndex < app->currentTab->fileOptions.length-1)
						{
							app->currentTab->selectedOptionIndex++;
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Right, false))
						{
							app->currentTab->selectedOptionIndex = 0;
							movedSelection = true;
						}
					}
				}
				else
				{
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Up, true))
					{
						if (app->currentTab->selectedOptionIndex > 0)
						{
							app->currentTab->selectedOptionIndex--;
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Up, false))
						{
							app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							movedSelection = true;
						}
					}
					if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Down, true))
					{
						if ((uxx)app->currentTab->selectedOptionIndex < app->currentTab->fileOptions.length-1)
						{
							app->currentTab->selectedOptionIndex++;
							movedSelection = true;
						}
						else if (IsKeyboardKeyPressed(&appIn->keyboard, Key_Down, false))
						{
							app->currentTab->selectedOptionIndex = 0;
							movedSelection = true;
						}
					}
				}
			}
			
			// Auto-scroll up/down to the newly selected option if needed
			if (movedSelection && app->currentTab->selectedOptionIndex >= 0)
			{
				rec viewportRec = GetClayElementDrawRec(CLAY_ID("OptionsList"));
				Clay_ScrollContainerData viewportScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
				FileOption* selectedOption = VarArrayGetHard(FileOption, &app->currentTab->fileOptions, (uxx)app->currentTab->selectedOptionIndex);
				Str8 btnIdStr = PrintInArenaStr(scratch, "%.*s_OptionBtn", StrPrint(selectedOption->name));
				ClayId btnId = ToClayIdEx(btnIdStr, (uxx)app->currentTab->selectedOptionIndex);
				rec optionRec = GetClayElementDrawRec(btnId);
				if (viewportScrollData.found && optionRec.Width > 0 && optionRec.Height > 0)
				{
					r32 maxScroll = MaxR32(0, viewportScrollData.contentDimensions.Height - viewportScrollData.scrollContainerDimensions.Height);
					r32 optionYPosition = (optionRec.Y - viewportRec.Y) - viewportScrollData.scrollPosition->Y;
					r32 scrollUpTarget = optionYPosition - (OPTIONS_AUTOSCROLL_BUFFER_ABOVE_BELOW * viewportRec.Height);
					r32 scrollDownTarget = optionYPosition + optionRec.Height - ((1.0f - OPTIONS_AUTOSCROLL_BUFFER_ABOVE_BELOW) * viewportRec.Height);
					if (-viewportScrollData.scrollTarget->Y < scrollDownTarget)
					{
						viewportScrollData.scrollTarget->Y = -MinR32(maxScroll, scrollDownTarget);
					}
					else if (-viewportScrollData.scrollTarget->Y > scrollUpTarget)
					{
						viewportScrollData.scrollTarget->Y = -MaxR32(0, scrollUpTarget);
					}
				}
			}
		}
	}
	else if (IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left) ||
		IsMouseBtnPressed(&appIn->mouse, MouseBtn_Right) ||
		IsMouseBtnPressed(&appIn->mouse, MouseBtn_Middle) ||
		(appIn->mouse.isOverWindow && !AreSimilarV2(appIn->mouse.scrollDelta, V2_Zero, DEFAULT_R32_TOLERANCE)))
	{
		app->usingKeyboardToSelect = false;
	}
	
	if (app->usingKeyboardToSelect && IsKeyboardKeyPressed(&appIn->keyboard, Key_Enter, false) &&
		app->currentTab != nullptr && app->currentTab->selectedOptionIndex >= 0)
	{
		FileOption* selectedOption = VarArrayGetHard(FileOption, &app->currentTab->fileOptions, (uxx)app->currentTab->selectedOptionIndex);
		if (selectedOption->type == FileOptionType_Bool)
		{
			ToggleOption(app->currentTab, selectedOption);
		}
	}
	
	// if (app->currentTab == nullptr)
	// {
	// 	UpdateClayTextbox(&app->testTextbox);
	// }
	
	// +==============================+
	// |          Rendering           |
	// +==============================+
	TracyCZoneEnd(Zone_Update);
	OsTime afterUpdateTime = OsGetTime();
	TracyCZoneN(Zone_BeginFrame, "BeginFrame", true);
	BeginFrame(platform->GetSokolSwapchain(), screenSizei, GetThemeColor(OptionListBack), 1.0f);
	TracyCZoneEnd(Zone_BeginFrame);
	OsTime beforeRenderTime = OsGetTime();
	{
		TracyCZoneN(Zone_Render, "Render", true);
		BindShader(&app->mainShader);
		ClearDepthBuffer(1.0f);
		SetDepth(1.0f);
		mat4 projMat = Mat4_Identity;
		TransformMat4(&projMat, MakeScaleXYZMat4(1.0f/(screenSize.Width/2.0f), 1.0f/(screenSize.Height/2.0f), 1.0f));
		TransformMat4(&projMat, MakeTranslateXYZMat4(-1.0f, -1.0f, 0.0f));
		TransformMat4(&projMat, MakeScaleYMat4(-1.0f));
		SetProjectionMat(projMat);
		SetViewMat(Mat4_Identity);
		
		uiArena = scratch3;
		FlagSet(uiArena->flags, ArenaFlag_DontPop);
		uxx uiArenaMark = ArenaGetMark(uiArena);
		UiWidgetContext uiContext = MakeUiWidgetContext(
			uiArena,
			&app->clay,
			&appIn->keyboard,
			&appIn->mouse,
			app->uiScale,
			nullptr, //TODO: Fill focusedElementPntr
			MouseCursorShape_Default,
			OsWindowHandleEmpty,
			appIn->programTime,
			&app->tooltips
		);
		
		v2 scrollContainerInput = IsKeyboardKeyDown(&appIn->keyboard, Key_Control) ? V2_Zero : appIn->mouse.scrollDelta;
		#if TARGET_IS_LINUX
		scrollContainerInput = ScaleV2(scrollContainerInput, LINUX_SCROLL_WHEEL_SCALING);
		#endif
		app->wasClayScrollingPrevFrame = UpdateClayScrolling(&app->clay.clay, 16.6f, false, scrollContainerInput, false);
		BeginClayUIRender(&app->clay.clay, screenSize, false, mousePos, IsMouseBtnDown(&appIn->mouse, MouseBtn_Left));
		{
			u16 fullscreenBorderThickness = (appIn->isWindowTopmost ? 1 : 0);
			CLAY({ .id = CLAY_ID("FullscreenContainer"),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = { .width = CLAY_SIZING_GROW(0, screenSize.Width), .height = CLAY_SIZING_GROW(0, screenSize.Height) },
					.padding = CLAY_PADDING_ALL(fullscreenBorderThickness)
				},
				.border = {
					.color=GetThemeColor(TopmostBorder),
					.width=CLAY_BORDER_OUTSIDE(UI_BORDER(fullscreenBorderThickness)),
				},
			})
			{
				// +==============================+
				// |            Topbar            |
				// +==============================+
				if (!app->minimalModeEnabled || app->isFileMenuOpen || app->isViewMenuOpen)
				{
					Clay_SizingAxis topbarHeight = app->minimalModeEnabled ? CLAY_SIZING_FIXED(0.1f) : CLAY_SIZING_FIT(0);
					CLAY({ .id = CLAY_ID("Topbar"),
						.layout = {
							.sizing = {
								.height = topbarHeight,
								.width = CLAY_SIZING_GROW(0),
							},
							.padding = { 0, 0, 0, UI_BORDER(1) },
							.childGap = 2,
							.childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
						},
						.backgroundColor = GetThemeColor(TopbarBack),
						.border = { .color=GetThemeColor(TopbarBorder), .width={ .bottom=UI_BORDER(1) } },
					})
					{
						bool showMenuHotkeys = IsKeyboardKeyDown(&appIn->keyboard, Key_Alt);
						if (ClayTopBtn("File", showMenuHotkeys, &app->isFileMenuOpen, &app->keepFileMenuOpenUntilMouseOver, app->isOpenRecentSubmenuOpen))
						{
							if (ClayBtn("Open" UNICODE_ELLIPSIS_STR, "Ctrl+O", "Open a file", true, &app->icons[AppIcon_OpenFile]))
							{
								shouldOpenFile = true;
								#if TARGET_IS_WINDOWS
								app->isFileMenuOpen = false;
								#endif
							} Clay__CloseElement();
							
							if (app->recentFiles.length > 0)
							{
								if (ClayTopSubmenu("Open Recent " UNICODE_RIGHT_ARROW_STR, app->isFileMenuOpen, &app->isOpenRecentSubmenuOpen, &app->keepOpenRecentSubmenuOpenUntilMouseOver, &app->icons[AppIcon_OpenRecent]))
								{
									for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
									{
										RecentFile* recentFile = VarArrayGet(RecentFile, &app->recentFiles, rIndex-1);
										Str8 displayPath = GetUniqueRecentFilePath(recentFile->path);
										bool isOpenFile = (AppFindTabForPath(recentFile->path) != nullptr);
										Str8 tooltipStr = PrintInArenaStr(uiArena, "%.*s%s", StrPrint(recentFile->path), recentFile->fileExists ? "" : " (MISSING)");
										if (ClayBtnStrEx(recentFile->path, AllocStr8(uiArena, displayPath), StrLit(""), tooltipStr, !isOpenFile && recentFile->fileExists, nullptr))
										{
											FileTab* newTab = AppOpenFileTab(recentFile->path);
											if (newTab != nullptr)
											{
												app->isOpenRecentSubmenuOpen = false;
												app->isFileMenuOpen = false;
											}
										} Clay__CloseElement();
									}
									
									Str8 clearRecentFilesTooltipStr = PrintInArenaStr(uiArena, "Remove %llu path%s from the \"Recent Files\" list", app->recentFiles.length, Plural(app->recentFiles.length, "s"));
									if (ClayBtnStr(StrLit("Clear Recent Files"), Str8_Empty, clearRecentFilesTooltipStr, app->recentFiles.length > 0, &app->icons[AppIcon_Trash]))
									{
										OpenPopupDialog(stdHeap, &app->popup,
											ScratchPrintStr("Are you sure you want to clear %s%llu recent file entr%s?", (app->recentFiles.length > 1) ? "all " : "", app->recentFiles.length, PluralEx(app->recentFiles.length, "y", "ies")),
											AppClearRecentFilesPopupCallback, nullptr
										);
										AddPopupButton(&app->popup, 1, StrLit("Cancel"), PopupDialogResult_No, GetThemeColor(ConfirmDialogNeutralBtnBorder));
										AddPopupButton(&app->popup, 2, StrLit("Clear Recent Files"), PopupDialogResult_Yes, GetThemeColor(ConfirmDialogNegativeBtnBorder));
										app->isOpenRecentSubmenuOpen = false;
										app->isFileMenuOpen = false;
									} Clay__CloseElement();
									
									Clay__CloseElement();
									Clay__CloseElement();
								} Clay__CloseElement();
							}
							else
							{
								if (ClayBtn("Open Recent " UNICODE_RIGHT_ARROW_STR, "", "", false, &app->icons[AppIcon_OpenRecent])) { } Clay__CloseElement();
							}
							
							if (ClayBtn("Reset File", "", "Reset file to how it was when first opened", (app->currentTab != nullptr && app->currentTab->isFileChangedFromOriginal), &app->icons[AppIcon_ResetFile]))
							{
								OpenPopupDialog(stdHeap, &app->popup,
									StrLit("Do you want to reset the file to the state it was in when it was opened?"),
									AppResetCurrentFilePopupCallback, nullptr
								);
								AddPopupButton(&app->popup, 1, StrLit("Cancel"), PopupDialogResult_No, GetThemeColor(ConfirmDialogNeutralBtnBorder));
								AddPopupButton(&app->popup, 2, StrLit("Reset"), PopupDialogResult_Yes, GetThemeColor(ConfirmDialogNegativeBtnBorder));
							} Clay__CloseElement();
							
							//TODO: Add an icon for this option
							if (ClayBtnStr(ScratchPrintStr("%s File Reloading", app->settings.dontAutoReloadFile ? "Enable" : "Disable"), Str8_Empty, StrLit("When an open file is changed externally, should CSwitch automatically read the new state of the file and display it. There is a small performance cost for watching the file for changes"), (app->tabs.length > 0), nullptr))
							{
								app->settings.dontAutoReloadFile = !app->settings.dontAutoReloadFile;
								SaveAppSettings();
							} Clay__CloseElement();
							
							if (ClayBtn("Close File", "Ctrl+W", "Close the current file tab", (app->currentTab != nullptr), &app->icons[AppIcon_CloseFile]))
							{
								AppCloseFileTab(app->currentTabIndex);
							} Clay__CloseElement();
							
							Clay__CloseElement();
							Clay__CloseElement();
						} Clay__CloseElement();
						
						if (ClayTopBtn("View", showMenuHotkeys, &app->isViewMenuOpen, &app->keepViewMenuOpenUntilMouseOver, false))
						{
							ThemeMode otherThemeMode = ((DEBUG_BUILD && IsKeyboardKeyDown(&appIn->keyboard, Key_Shift))
								? ThemeMode_Debug
								: ((app->currentThemeMode == ThemeMode_Dark)
									? ThemeMode_Light
									: ThemeMode_Dark
								)
							);
							if (ClayBtnStr(ScratchPrintStr("%s Mode", GetThemeModeStr(otherThemeMode)), StrLit("F9"), StrLit("Toggle between dark and light mode"), true, &app->icons[AppIcon_LightDark]))
							{
								app->currentThemeMode = otherThemeMode;
								SetAppSettingStr8Pntr(&app->settings, &app->settings.themeMode, MakeStr8Nt(GetThemeModeStr(app->currentThemeMode)));
								SaveAppSettings();
								AppBakeTheme(false);
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("%s Buttons", app->settings.smallButtons ? "Large" : "Small"), StrLit("F10"), StrLit("Toggle between small buttons with abbreviations laid out in a grid and large buttons with full names in a vertical list"), true, &app->icons[AppIcon_SmallBtn]))
							{
								app->settings.smallButtons = !app->settings.smallButtons;
								SaveAppSettings();
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "Disable" : "Enable"), StrLit("Ctrl+T"), StrLit("Toggle forcing this window to display on top of other windows even when it's not focused (Windows only)"), true, &app->icons[appIn->isWindowTopmost ? AppIcon_TopmostEnabled : AppIcon_TopmostDisabled]))
							{
								#if TARGET_IS_WINDOWS
								platform->SetWindowTopmost(!appIn->isWindowTopmost);
								#else
								Notify_W("Topmost toggling is only implemented for Windows.\nOn some Linux distributions you can toggle this yourself by right-clicking on the window topbar  ");
								#endif
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("Clip Names on %s", app->settings.clipNamesLeft ? "Left" : "Right"), Str8_Empty, StrLit("Changes which side of the full name we should omit on a button when there is not enough horizontal space"), true, &app->icons[app->settings.clipNamesLeft ? AppIcon_ClipLeft : AppIcon_ClipRight]))
							{
								app->settings.clipNamesLeft = !app->settings.clipNamesLeft;
								SaveAppSettings();
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("%s Smooth Scrolling", app->settings.smoothScrollingDisabled ? "Enable" : "Disable"), Str8_Empty, StrLit("Toggles whether the scrollable list should animate over time after being moved with mouse scroll wheel"), true, &app->icons[AppIcon_SmoothScroll]))
							{
								app->settings.smoothScrollingDisabled = !app->settings.smoothScrollingDisabled;
								SaveAppSettings();
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("%s Option Tooltips", app->settings.optionTooltipsDisabled ? "Enable" : "Disable"), Str8_Empty, StrLit("Toggles whether tooltips with full name should be displayed when hovering over a button in the list"), true, &app->icons[AppIcon_Tooltip]))
							{
								app->settings.optionTooltipsDisabled = !app->settings.optionTooltipsDisabled;
								SaveAppSettings();
							} Clay__CloseElement();
							
							if (ClayBtnStr(ScratchPrintStr("%s Topbar", app->minimalModeEnabled ? "Show" : "Hide"), StrLit("F11"), StrLit("Toggles visibility of this topbar, usually only useful if we need to maximize use of vertical space"), true, &app->icons[AppIcon_Topbar]))
							{
								app->minimalModeEnabled = !app->minimalModeEnabled;
							} Clay__CloseElement();
							
							Str8 currentThemeStr = !IsEmptyStr(app->settings.userThemePath)
								? PrintInArenaStr(uiArena, "current: \"%.*s\"", StrPrint(app->settings.userThemePath))
								: StrLit("current: -");
							Str8 openThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Open a custom user theme file"), '\n', currentThemeStr, false);
							if (ClayBtnStr(StrLit("Open Custom Theme" UNICODE_ELLIPSIS_STR), Str8_Empty, openThemeTooltipStr, true, &app->icons[AppIcon_StarFile]))
							{
								shouldOpenThemeFile = true;
								app->isFileMenuOpen = false;
							} Clay__CloseElement();
							
							Str8 clearThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Remove the custom user theme"), '\n', currentThemeStr, false);
							if (ClayBtnStr(StrLit("Clear Custom Theme"), Str8_Empty, clearThemeTooltipStr, !IsEmptyStr(app->settings.userThemePath), &app->icons[AppIcon_CloseFile]))
							{
								SetAppSettingStr8Pntr(&app->settings, &app->settings.userThemePath, Str8_Empty);
								SaveAppSettings();
								AppLoadUserTheme();
								AppBakeTheme(false);
							} Clay__CloseElement();
							
							#if DEBUG_BUILD
							if (ClayBtnStr(ScratchPrintStr("%s Clay UI Debug", Clay_IsDebugModeEnabled() ? "Hide" : "Show"), StrLit("~"), StrLit("Toggles the debug sidebar for Clay"), true, &app->icons[AppIcon_Debug]))
							{
								Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
							} Clay__CloseElement();
							if (ClayBtnStr(ScratchPrintStr("%s Sleeping", app->sleepingDisabled ? "Enable" : "Disable"), Str8_Empty, StrLit("Toggles whether the rendering loop is allowed to \"sleep\" when nothing is changing"), true, nullptr))
							{
								app->sleepingDisabled = !app->sleepingDisabled;
							} Clay__CloseElement();
							if (ClayBtnStr(ScratchPrintStr("%s Frame Indicator", app->enableFrameUpdateIndicator ? "Disable" : "Enable"), Str8_Empty, StrLit("Toggles a indicator that changes every frame, helpful when debugging \"sleep\" behavior or trying to visualize the framerate"), true, nullptr))
							{
								app->enableFrameUpdateIndicator = !app->enableFrameUpdateIndicator;
							} Clay__CloseElement();
							#endif //DEBUG_BUILD
							
							// if (ClayBtn("Close Window", "Ctrl+Shift+W", true, &app->icons[AppIcon_CloseWindow]))
							// {
							// 	platform->RequestQuit();
							// 	app->isFileMenuOpen = false;
							// } Clay__CloseElement();
							
							Clay__CloseElement();
							Clay__CloseElement();
						} Clay__CloseElement();
						
						CLAY({ .layout = { .sizing = { .width=CLAY_SIZING_GROW(0) } } }) {}
						
						if (app->currentTab != nullptr)
						{
							// app->filePathTooltipId2 = SoftRegisterTooltip(&app->tooltips, app->filePathTooltipId2, StrLit("FilePathDisplay"), Rec_Zero, app->currentTab->filePath, &app->uiFont, app->uiFontSize, UI_FONT_STYLE);
							
							Str8 filePathScratch = AllocStr8(uiArena, app->currentTab->filePath);
							CLAY({ .id = CLAY_ID("FilePathDisplay"),
								.tooltip = {
									.text = filePathScratch,
									.fontId = app->clayUiFontId,
									.fontSize = (u16)app->uiFontSize,
								},
							})
							{
								CLAY_TEXT(
									filePathScratch,
									CLAY_TEXT_CONFIG({
										.fontId = app->clayUiFontId,
										.fontSize = (u16)app->uiFontSize,
										.textColor = GetThemeColor(TopbarPathText),
										.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
										.wrapMode = CLAY_TEXT_WRAP_NONE,
										.userData = { .contraction = TextContraction_EllipseFilePath },
									})
								);
							}
							CLAY({ .layout={ .sizing={ .width=CLAY_SIZING_FIXED(UI_R32(4)) } } }) {}
						}
						
						#if DEBUG_BUILD
						if (app->enableFrameUpdateIndicator)
						{
							//NOTE: This little visual makes it easier to tell when we are rendering new frames and when we are asleep by having a little bar move every frame
							CLAY({ .id=CLAY_ID("FrameUpdateIndicatorContainer"),
								.layout = {
									.sizing={ .width=CLAY_SIZING_FIXED(UI_R32(4)), .height=CLAY_SIZING_GROW(0) },
									.layoutDirection = CLAY_TOP_TO_BOTTOM,
								},
								.backgroundColor = Black,
							})
							{
								for (uxx pIndex = 0; pIndex < 10; pIndex++)
								{
									CLAY({
										.layout = { .sizing = { .width=CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(0.1f) } },
										.backgroundColor = ((appIn->frameIndex % 10) == pIndex) ? MonokaiWhite : Black,
									}) {}
								}
							}
						}
						#endif
					}
				}
				
				// +==============================+
				// |             Tabs             |
				// +==============================+
				if (app->tabs.length > 1)
				{
					CLAY({ .id = CLAY_ID("TabGutter"),
						.layout = {
							.layoutDirection = CLAY_LEFT_TO_RIGHT,
							.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
							.padding = { .top = UI_U16(4) },
						},
						.backgroundColor = GetThemeColor(TopbarBack),
					})
					{
						bool wasPrevHovered = false;
						VarArrayLoop(&app->tabs, tIndex)
						{
							VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
							bool isCurrentTab = (app->currentTabIndex == tIndex);
							ClayId tabId = ToClayId(tab->filePath);
							bool isHovered = IsMouseOverClay(tabId);
							ThemeState tabThemeState = isCurrentTab ? ThemeState_Open : (isHovered ? ThemeState_Hovered : ThemeState_Default);
							Color32 backgroundColor = GetThemeColorEx(FileTabBack,   tabThemeState);
							Color32 borderColor     = GetThemeColorEx(FileTabBorder, tabThemeState);
							Color32 textColor       = GetThemeColorEx(FileTabText,   tabThemeState);
							
							// Dividers in-between not-selected and not-hovered tabs
							if (tIndex > 0)
							{
								bool shouldShowDivider = (!isCurrentTab && app->currentTabIndex != tIndex-1 && !isHovered && !wasPrevHovered);
								CLAY({
									.layout = {
										.sizing = { .width = CLAY_SIZING_FIXED(UI_R32(1)), .height = CLAY_SIZING_GROW(0) },
									},
									.backgroundColor = (shouldShowDivider ? GetThemeColor(FileTabDivider) : Transparent),
								}) {}
							}
							
							CLAY({ .id = tabId,
								.layout = {
									.layoutDirection = CLAY_LEFT_TO_RIGHT,
									.sizing = { .width = CLAY_SIZING_PERCENT(1.0f / (r32)app->tabs.length) },
									.padding = CLAY_PADDING_ALL(UI_U16(4)),
								},
								.cornerRadius = { .topLeft=UI_U16(4), .topRight=UI_U16(4), .bottomLeft=0, .bottomRight=0 },
								.backgroundColor = backgroundColor,
								.border = {
									.color = borderColor,
									.width = { .left=UI_BORDER(2), .top=UI_BORDER(2), .right=UI_BORDER(2), .bottom=0, }, //TODO: Add support to Clay Renderer for missing sides when both corners don't have a radius!
								},
							})
							{
								CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
								Str8 displayPath = GetUniqueTabFilePath(tab->filePath);
								Str8 displayPathAlloced = AllocStr8(uiArena, displayPath);
								CLAY_TEXT(
									displayPathAlloced,
									CLAY_TEXT_CONFIG({
										.fontId = app->clayUiFontId,
										.fontSize = (u16)app->uiFontSize,
										.textColor = textColor,
										.wrapMode = CLAY_TEXT_WRAP_NONE,
										.textAlignment = CLAY_TEXT_ALIGN_SHRINK,
										.userData = { .contraction = TextContraction_EllipseRight },
									})
								);
								CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
								
								if (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Left))
								{
									AppChangeTab(tIndex);
								}
								if (isHovered && IsMouseBtnPressed(&appIn->mouse, MouseBtn_Middle))
								{
									AppCloseFileTab(tIndex);
									tIndex--;
								}
							}
							
							wasPrevHovered = isHovered;
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
					ClayId optionsContainerId = CLAY_ID("OptionsList");
					CLAY({ .id = optionsContainerId,
						.layout = {
							.layoutDirection = CLAY_TOP_TO_BOTTOM,
							.childGap = UI_U16(OPTION_UI_GAP),
							.padding = CLAY_PADDING_ALL(UI_U16(4)),
							.sizing = {
								.height = CLAY_SIZING_GROW(0),
								.width = CLAY_SIZING_GROW(0)
							},
						},
						.scroll = { .vertical=true, .scrollLag = app->settings.smoothScrollingDisabled ? 0.0f : (r32)OPTIONS_SMOOTH_SCROLLING_DIVISOR },
					})
					{
						if (app->currentTab != nullptr)
						{
							if (app->settings.smallButtons)
							{
								// rec optionsDrawRec = GetClayElementDrawRec(optionsContainerId);
								r32 optionsAreaWidth = screenSize.Width - (app->minimalModeEnabled ? 0.0f : UI_R32(SCROLLBAR_WIDTH)) - (r32)(UI_U16(4) * 2);
								u16 buttonMargin = UI_U16(SMALL_BTN_MARGIN);
								r32 buttonWidth = app->currentTab->longestAbbreviationWidth + (r32)UI_U16(SMALL_BTN_PADDING_X)*2;
								i32 numColumns = FloorR32i((optionsAreaWidth - (r32)buttonMargin) / (buttonWidth + (r32)buttonMargin));
								if (numColumns <= 0) { numColumns = 1; }
								
								bool containerStarted = false;
								VarArrayLoop(&app->currentTab->fileOptions, oIndex)
								{
									VarArrayLoopGet(FileOption, option, &app->currentTab->fileOptions, oIndex);
									bool isOptionSelected = (app->usingKeyboardToSelect && app->currentTab->selectedOptionIndex >= 0 && (uxx)app->currentTab->selectedOptionIndex == oIndex);
									
									if ((oIndex % numColumns) == 0)
									{
										if (containerStarted) { Clay__CloseElement(); }
										Clay__OpenElement();
										Clay__ConfigureOpenElement((Clay_ElementDeclaration){
											.layout = {
												.layoutDirection = CLAY_LEFT_TO_RIGHT,
												.childGap = buttonMargin,
												.sizing = { .width = CLAY_SIZING_PERCENT(1.0f) },
												.childAlignment = { .x = CLAY_ALIGN_X_CENTER },
											},
										});
										containerStarted = true;
									}
									
									if (option->type == FileOptionType_Bool)
									{
										if (ClaySmallOptionBtn(optionsContainerId, buttonWidth, option->name, oIndex, option->abbreviation, option->valueBool, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
									else if (option->type == FileOptionType_CommentDefine)
									{
										if (ClaySmallOptionBtn(optionsContainerId, buttonWidth, option->name, oIndex, option->abbreviation, option->isUncommented, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
									else
									{
										if (ClaySmallOptionBtn(optionsContainerId, buttonWidth, option->name, oIndex, option->abbreviation, false, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
								}
								if (containerStarted) { Clay__CloseElement(); }
							}
							else
							{
								VarArrayLoop(&app->currentTab->fileOptions, oIndex)
								{
									VarArrayLoopGet(FileOption, option, &app->currentTab->fileOptions, oIndex);
									bool isOptionSelected = (app->usingKeyboardToSelect && app->currentTab->selectedOptionIndex >= 0 && (uxx)app->currentTab->selectedOptionIndex == oIndex);
									
									if (option->type == FileOptionType_Bool)
									{
										//NOTE: We have to put a copy of valueStr in uiArena because the current valueStr might be deallocated before the end of the frame when Clay needs to render the text!
										if (ClayOptionBtn(optionsContainerId, option->name, oIndex, option->name, PrintInArenaStr(uiArena, "%.*s", StrPrint(option->valueStr)), option->valueBool, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
									else if (option->type == FileOptionType_CommentDefine)
									{
										if (ClayOptionBtn(optionsContainerId, option->name, oIndex, ScratchPrintStr("%s%.*s", option->isUncommented ? "" : "// ", StrPrint(option->name)), Str8_Empty, option->isUncommented, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
									else
									{
										if (ClayOptionBtn(optionsContainerId, option->name, oIndex, option->name, StrLit("-"), false, isOptionSelected))
										{
											ToggleOption(app->currentTab, option);
										}
									}
									if (option->numEmptyLinesAfter > 0)
									{
										CLAY({ .layout = { .sizing = { .height=CLAY_SIZING_FIXED(UI_R32((r32)option->numEmptyLinesAfter * LINE_BREAK_EXTRA_UI_GAP)) } } }) {}
									}
								}
							}
						}
						
						// if (app->currentTab == nullptr)
						// {
						// 	DrawClayTextbox(&app->testTextbox, CLAY_SIZING_GROW(0));
						// }
						
						#if 0
						if (app->currentTab == nullptr)
						{
							CLAY({
								.layout = { .padding = CLAY_PADDING_ALL(UI_BORDER(1)), .layoutDirection = CLAY_LEFT_TO_RIGHT, },
								.border = { .color = GetThemeColor(TextLightGray), .width=CLAY_BORDER_ALL(UI_BORDER(1)) },
							})
							{
								FontAtlas* uiAtlas = GetFontAtlas(&app->uiFont, app->uiFontSize, UI_FONT_STYLE);
								NotNull(uiAtlas);
								FontAtlas* mainAtlas = GetFontAtlas(&app->mainFont, app->mainFontSize, MAIN_FONT_STYLE);
								NotNull(mainAtlas);
								CLAY({ .id = CLAY_ID("UiFontAtlas"),
									.layout = {
										.sizing = { .width = CLAY_SIZING_FIXED(UI_R32(uiAtlas->texture.Width)), .height = CLAY_SIZING_FIXED(UI_R32(uiAtlas->texture.Height)) },
									},
									.image = ToClayImage(&uiAtlas->texture),
								}) {}
								CLAY({ .id = CLAY_ID("MainFontAtlas"),
									.layout = {
										.sizing = { .width = CLAY_SIZING_FIXED(UI_R32(mainAtlas->texture.Width)), .height = CLAY_SIZING_FIXED(UI_R32(mainAtlas->texture.Height)) },
									},
									.image = ToClayImage(&mainAtlas->texture),
								}) {}
							}
						}
						#endif
					}
					
					if (app->currentTab != nullptr)
					{
						ClayScrollbar(optionsContainerId, StrLit("OptionsListScrollbar"), app->minimalModeEnabled ? 0.0f : UI_R32(SCROLLBAR_WIDTH), &app->currentTab->scrollbarState);
					}
				}
				
				RenderPopupDialog(&app->popup);
				DoUiNotificationQueue(&uiContext, &app->notificationQueue, &app->uiFont, app->uiFontSize, UI_FONT_STYLE, appIn->screenSize);
				DoUiTooltips(&uiContext, &app->tooltips, screenSize);
			}
		}
		
		Clay_RenderCommandArray clayRenderCommands = EndClayUIRender(&app->clay.clay);
		RenderClayCommandArray(&app->clay, &gfx, &clayRenderCommands);
		FlagUnset(uiArena->flags, ArenaFlag_DontPop);
		ArenaResetToMark(uiArena, uiArenaMark);
		uiArena = nullptr;
		
		// +==============================+
		// |  Closed File Debug Displays  |
		// +==============================+
		#if DEBUG_BUILD
		if (app->currentTab == nullptr)
		{
			DrawClayTextboxText(&app->testTextbox);
			
			#if 0
			DrawSheetCell(&app->testSheet, MakeV2i(1, 0), MakeRec(10, 300, 64, 64), White);
			DrawSheetCell(&app->testSheet, MakeV2i(0, 1), MakeRec(10, 364, 64, 64), White);
			for (u64 index = 0; index < 128; index++)
			{
				DrawNamedSheetCell(&app->testSheet, StrLit("error"), MakeRec(10 + 2.0f*index, 428, 64, 64), ((index%2) == 0) ? MonokaiBlue : White);
			}
			#endif
			
			app->testTooltipId = SoftRegisterTooltip(&app->tooltips, app->testTooltipId, Str8_Empty, 0, MakeRec(10, 60, 100, 100), StrLit("This is a test!"), &app->uiFont, app->uiFontSize, UI_FONT_STYLE);
			
			RegisteredTooltip* hoverTooltip = TryFindRegisteredTooltip(&app->tooltips, app->tooltips.hoverTooltipId);
			Str8 hoverTooltipStr = (hoverTooltip != nullptr) ? hoverTooltip->displayStr : Str8_Empty;
			RegisteredTooltip* openTooltip = TryFindRegisteredTooltip(&app->tooltips, app->tooltips.openTooltipId);
			Str8 openTooltipStr = (openTooltip != nullptr) ? openTooltip->displayStr : Str8_Empty;
			BindFont(&app->uiFont);
			v2 textPos = MakeV2(10, 200);
			DrawText(PrintInArenaStr(scratch, "Tooltips: %llu registered", app->tooltips.tooltips.length), textPos, GetThemeColor(OptionOnNameText)); textPos.Y += GetLineHeight();
			DrawText(PrintInArenaStr(scratch, "HoveredTooltip: %llu \"%.*s\"", app->tooltips.hoverTooltipId, StrPrint(hoverTooltipStr)), textPos, GetThemeColor(OptionOnNameText)); textPos.Y += GetLineHeight();
			DrawText(PrintInArenaStr(scratch, "OpenTooltip: %llu \"%.*s\"", app->tooltips.openTooltipId, StrPrint(openTooltipStr)), textPos, GetThemeColor(OptionOnNameText)); textPos.Y += GetLineHeight();
			DrawText(PrintInArenaStr(scratch, "HoverChanged: %llums ago", TimeSinceBy(appIn->programTime, app->tooltips.hoverTooltipChangeTime)), textPos, GetThemeColor(OptionOnNameText)); textPos.Y += GetLineHeight();
			DrawText(PrintInArenaStr(scratch, "MouseMove: %llums ago", TimeSinceBy(appIn->programTime, app->tooltips.lastMouseMoveTime)), textPos, GetThemeColor(OptionOnNameText)); textPos.Y += GetLineHeight();
		}
		#endif
		
		#if 0
		FontAtlas* fontAtlas = GetFontAtlas(&app->uiFont, app->uiFontSize, UI_FONT_STYLE);
		BindFontEx(&app->uiFont, app->uiFontSize, UI_FONT_STYLE);
		rec textRec = MakeRec(50, screenSize.Height/2, mousePos.X - 50, fontAtlas->lineHeight);
		if (textRec.Width < 5) { textRec.Width = 5; }
		DrawRectangleOutline(textRec, 2, MonokaiRed);
		Str8 shortenedPath = ShortenFilePathToFitWidth(scratch, &app->uiFont, app->uiFontSize, UI_FONT_STYLE, app->filePath, textRec.Width, StrLit("..."));
		DrawText(shortenedPath, MakeV2(textRec.X, textRec.Y + textRec.Height/2 + fontAtlas->centerOffset), MonokaiWhite);
		#endif
		
		// +==============================+
		// |       Render Overlays        |
		// +==============================+
		if (app->showPerfGraph)
		{
			RenderPerfGraph(&app->perfGraph, &gfx, &app->uiFont, app->uiFontSize, UI_FONT_STYLE, MakeRec(10, 10, 400, 100));
		}
		
		TracyCZoneEnd(Zone_Render);
	}
	CommitAllFontTextureUpdates(&app->uiFont);
	CommitAllFontTextureUpdates(&app->mainFont);
	OsTime afterRenderTime = OsGetTime();
	TracyCZoneN(Zone_EndFrame, "BeginFrame", true);
	EndFrame();
	TracyCZoneEnd(Zone_EndFrame);
	app->renderedLastFrame = true;
	app->prevUpdateMs =
		OsTimeDiffMsR32(beforeUpdateTime, afterUpdateTime) +
		OsTimeDiffMsR32(beforeRenderTime, afterRenderTime);
	
	// +==============================+
	// |   Handle Open File Dialog    |
	// +==============================+
	if (shouldOpenFile || shouldOpenThemeFile)
	{
		#if TARGET_IS_WINDOWS
		Str8 selectedPath = Str8_Empty;
		Result openResult = OsDoOpenFileDialog(scratch, &selectedPath);
		if (openResult == Result_Success)
		{
			PrintLine_I("Opened \"%.*s\"", StrPrint(selectedPath));
			if (shouldOpenFile) { AppOpenFileTab(selectedPath); }
			else if (shouldOpenThemeFile)
			{
				SetAppSettingStr8Pntr(&app->settings, &app->settings.userThemePath, selectedPath);
				if (AppLoadUserTheme())
				{
					SaveAppSettings();
					AppBakeTheme(true);
				}
			}
		}
		else if (openResult == Result_Canceled) { WriteLine_D("Canceled..."); }
		else { NotifyPrint_E("OpenDialog error: %s", GetResultStr(openResult)); }
		#else //!TARGET_IS_WINDOWS
		Notify_W("Open File dialog is not implemented on Linux currently! Please use drag-and-drop or pass the file path as a command-line argument!");
		#endif
	}
	
	ScratchEnd(scratch);
	ScratchEnd(scratch2);
	ScratchEnd(scratch3);
	
	TracyCZoneEnd(_funcZone);
	return true;
}

// +==============================+
// |          AppClosing          |
// +==============================+
// void AppClosing(PlatformInfo* inPlatformInfo, PlatformApi* inPlatformApi, void* memoryPntr)
EXPORT_FUNC APP_CLOSING_DEF(AppClosing)
{
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	UpdateDllGlobals(inPlatformInfo, inPlatformApi, memoryPntr, nullptr);
	
	#if THREAD_POOL_TEST
	FreeThreadPool(&app->threadPool);
	#endif
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
EXPORT_FUNC APP_GET_API_DEF(AppGetApi)
{
	AppApi result = ZEROED;
	result.AppInit = AppInit;
	result.AppUpdate = AppUpdate;
	result.AppClosing = AppClosing;
	result.AppBeforeReload = AppBeforeReload;
	result.AppAfterReload = AppAfterReload;
	return result;
}
