/*
File:   platform_main.c
Author: Taylor Robbins
Date:   01\19\2025
Description: 
	** Holds the main entry point for the application and #includes all source files
	** that are needed for the platform layer to be compiled to an executable.
	** If BUILD_INTO_SINGLE_UNIT then this file #includes app_main.c as well.
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

#if BUILD_WITH_RAYLIB
#include "third_party/raylib/raylib.h"
#endif

#include "lib/lib_sokol_app_impl.c"

#define ENABLE_RAYLIB_LOGS_DEBUG   0
#define ENABLE_RAYLIB_LOGS_INFO    0
#define ENABLE_RAYLIB_LOGS_WARNING 1
#define ENABLE_RAYLIB_LOGS_ERROR   1

#define APP_DLL_RELOAD_DELAY       500 //ms

#define MIN_ELAPSED_MS 5  //ms
#define MAX_ELAPSED_MS 67 //ms
#define TIME_SCALE_TARGET_FRAMERATE 60 //fps
#define TIME_SCALE_ROUND_TOLERANCE 0.1

// +--------------------------------------------------------------+
// |                         Header Files                         |
// +--------------------------------------------------------------+
#include "platform_interface.h"
#include "platform_main.h"

#if BUILD_INTO_SINGLE_UNIT
EXPORT_FUNC APP_GET_API_DEF(AppGetApi);
#endif

// +--------------------------------------------------------------+
// |                       Platform Globals                       |
// +--------------------------------------------------------------+
PlatformData* platformData = nullptr;
//These globals are shared between app and platform when BUILD_INTO_SINGLE_UNIT
Arena* stdHeap = nullptr;
PlatformInfo* platformInfo = nullptr;
PlatformApi* platform = nullptr;
ProgramArgs programArgs = ZEROED;

// +--------------------------------------------------------------+
// |                    Platform Source Files                     |
// +--------------------------------------------------------------+
#include "platform_api.c"
#include "platform_helpers.c"

#if BUILD_WITH_RAYLIB
void RaylibLogCallback(int logLevel, const char* text, va_list args)
{
	DbgLevel dbgLevel;
	switch (logLevel)
	{
		case LOG_TRACE:   dbgLevel = DbgLevel_Debug;   break;
		case LOG_DEBUG:   dbgLevel = DbgLevel_Debug;   break;
		case LOG_INFO:    dbgLevel = DbgLevel_Info;    break;
		case LOG_WARNING: dbgLevel = DbgLevel_Warning; break;
		case LOG_ERROR:   dbgLevel = DbgLevel_Error;   break;
		case LOG_FATAL:   dbgLevel = DbgLevel_Error;   break;
		default: dbgLevel = DbgLevel_Regular; break;
	}
	if (dbgLevel == DbgLevel_Debug && !ENABLE_RAYLIB_LOGS_DEBUG) { return; }
	if (dbgLevel == DbgLevel_Info && !ENABLE_RAYLIB_LOGS_INFO) { return; }
	if (dbgLevel == DbgLevel_Warning && !ENABLE_RAYLIB_LOGS_WARNING) { return; }
	if (dbgLevel == DbgLevel_Error && !ENABLE_RAYLIB_LOGS_ERROR) { return; }
	
	ScratchBegin(scratch);
	va_list argsCopy;
	va_copy(argsCopy, args);
	char* formattedText = nullptr;
	int formattedTextLength = MyVaListPrintf(nullptr, 0, text, args);
	if (formattedTextLength >= 0)
	{
		formattedText = AllocArray(char, scratch, formattedTextLength+1);
		if (formattedText != nullptr)
		{
			MyVaListPrintf(formattedText, formattedTextLength+1, text, argsCopy);
			formattedText[formattedTextLength] = '\0';
		}
	}
	va_end(argsCopy);
	if (formattedText != nullptr)
	{
		WriteLineAt(dbgLevel, formattedText);
	}
	else
	{
		WriteLine_E("RaylibLogCallback PRINT FAILURE!");
		WriteLineAt(dbgLevel, text);
	}
	ScratchEnd(scratch);
}
#endif //BUILD_WITH_RAYLIB

void PlatUpdateAppInputTimingInfo(AppInput* appInput)
{
	OsTime currentTime = OsGetTime();
	OsTime prevTime = platformData->prevFrameTime;
	if (appInput->frameIndex == 0) { prevTime = currentTime; } //ignore difference between 0 and first frame time
	platformData->prevFrameTime = currentTime;
	
	appInput->programTime = currentTime.msSinceStart;
	appInput->programTimeRemainder = currentTime.msSinceStartRemainder;
	
	r32 elapsedMsRemainder = 0.0f;
	u64 elapsedMs = OsTimeDiffMsU64(prevTime, currentTime, &elapsedMsRemainder);
	appInput->unclampedElapsedMsR64 = (r64)elapsedMs + (r64)elapsedMsRemainder;
	if (elapsedMs < MIN_ELAPSED_MS) { elapsedMs = MIN_ELAPSED_MS; elapsedMsRemainder = 0.0f; }
	else if (elapsedMs > MAX_ELAPSED_MS) { elapsedMs = MAX_ELAPSED_MS; elapsedMsRemainder = 0.0f; }
	else if (elapsedMs == MAX_ELAPSED_MS && elapsedMsRemainder > 0.0f) { elapsedMsRemainder = 0.0f; }
	appInput->elapsedMsR64 = (r64)elapsedMs + (r64)elapsedMsRemainder;
	appInput->elapsedMs = (r32)appInput->elapsedMsR64;
	
	appInput->timeScaleR64 = appInput->elapsedMsR64 / (1000.0 / TIME_SCALE_TARGET_FRAMERATE);
	if (AreSimilarR64(appInput->timeScaleR64, 1.0, TIME_SCALE_ROUND_TOLERANCE)) { appInput->timeScaleR64 = 1.0; }
	appInput->timeScale = (r32)appInput->timeScaleR64;
}

bool PlatDoUpdate(void)
{
	TracyCFrameMarkNamed("Game Loop");
	TracyCZoneN(_funcZone, "PlatDoUpdate", true);
	OsTime beforeUpdateTime = OsGetTime();
	bool renderedFrame = true;
	
	#if !BUILD_INTO_SINGLE_UNIT
	u64 programTime = platformData->currentAppInput->programTime; //TODO: Should we source this from somewhere else?
	OsUpdateFileWatch(&platformData->appDllWatch, programTime);
	if (platformData->appDllWatch.change != OsFileWatchChange_None)
	{
		if (!platformData->appDllWatch.fileExists) //ignore when the file gets deleted
		{
			PrintLine_W("The application DLL at \"%.*s\" has been deleted!", StrPrint(platformData->appDllPath));
			OsResetFileWatch(&platformData->appDllWatch, programTime);
		}
		else if (TimeSinceBy(programTime, platformData->appDllWatch.lastChangeTime) >= APP_DLL_RELOAD_DELAY)
		{
			OsResetFileWatch(&platformData->appDllWatch, programTime);
			
			if (platformData->appApi.AppBeforeReload(platformInfo, platform, platformData->appMemoryPntr))
			{
				OsUnloadDll(&platformData->appDll);
				ClearStruct(platformData->appApi);
				
				bool copyResult = OsCopyFile(platformData->appDllPath, platformData->appDllTempPath);
				Assert(copyResult == true);
				
				OsDll newDll;
				Result loadDllResult = OsLoadDll(platformData->appDllTempPath, &newDll);
				if (loadDllResult != Result_Success)
				{
					NotifyPrint_E("Failed to load \"%.*s\": %s", StrPrint(platformData->appDllPath), GetResultStr(loadDllResult));
					AssertMsg(loadDllResult == Result_Success, "App DLL reload failed!");
				}
				
				platformData->appDll = newDll;
				AppGetApi_f* appGetApi = (AppGetApi_f*)OsFindDllFunc(&newDll, StrLit("AppGetApi"));
				NotNull(appGetApi);
				platformData->appApi = appGetApi();
				NotNull(platformData->appApi.AppInit);
				NotNull(platformData->appApi.AppUpdate);
				NotNull(platformData->appApi.AppClosing);
				NotNull(platformData->appApi.AppBeforeReload);
				NotNull(platformData->appApi.AppAfterReload);
				
				platformData->appApi.AppAfterReload(platformInfo, platform, platformData->appMemoryPntr);
			}
			else { WriteLine_W("Ignoring application DLL change because current App blocked the reload"); }
		}
	}
	#endif
	
	//Swap which appInput is being written to and pass the static version to the application
	AppInput* oldAppInput = platformData->currentAppInput;
	AppInput* newAppInput = (platformData->currentAppInput == &platformData->appInputs[0]) ? &platformData->appInputs[1] : &platformData->appInputs[0];
	
	#if BUILD_WITH_RAYLIB
	v2i newScreenSize = MakeV2i((i32)GetRenderWidth(), (i32)GetRenderHeight());
	bool newIsFullScreen = IsWindowFullscreen();
	bool isMouseLocked = IsCursorHidden();
	#elif BUILD_WITH_SOKOL_APP
	v2i newScreenSize = MakeV2i(sapp_width(), sapp_height());
	bool newIsFullScreen = sapp_is_fullscreen();
	bool isMouseLocked = sapp_mouse_locked();
	#else
	v2i newScreenSize = MakeV2i(800, 600);
	bool newIsFullScreen = false;
	bool isMouseLocked = false;
	#endif
	PlatUpdateAppInputTimingInfo(oldAppInput);
	
	if (!AreEqual(newScreenSize, oldAppInput->screenSize)) { oldAppInput->screenSizeChanged = true; }
	oldAppInput->screenSize = newScreenSize;
	if (oldAppInput->isFullscreen != newIsFullScreen) { oldAppInput->isFullscreenChanged = true; }
	oldAppInput->isFullscreen = newIsFullScreen;
	
	VarArrayLoop(&newAppInput->droppedFilePaths, fIndex)
	{
		VarArrayLoopGet(Str8, filePathStr, &newAppInput->droppedFilePaths, fIndex);
		FreeStr8(stdHeap, filePathStr);
	}
	MyMemCopy(newAppInput, oldAppInput, sizeof(AppInput));
	VarArrayClear(&newAppInput->droppedFilePaths);
	newAppInput->screenSizeChanged = false;
	newAppInput->isFullscreenChanged = false;
	newAppInput->isMinimizedChanged = false;
	newAppInput->isFocusedChanged = false;
	RefreshKeyboardState(&newAppInput->keyboard);
	RefreshMouseState(&newAppInput->mouse, isMouseLocked, MakeV2((r32)newScreenSize.Width/2.0f, (r32)newScreenSize.Height/2.0f));
	IncrementU64(newAppInput->frameIndex);
	platformData->oldAppInput = oldAppInput;
	platformData->currentAppInput = newAppInput;
	OsTime afterUpdateTime = OsGetTime();
	platformInfo->updateMs = OsTimeDiffMsR32(beforeUpdateTime, afterUpdateTime);
	
	renderedFrame = platformData->appApi.AppUpdate(platformInfo, platform, platformData->appMemoryPntr, oldAppInput);
	
	TracyCZoneEnd(_funcZone);
	return renderedFrame;
}

// +--------------------------------------------------------------+
// |                   Platform Initialization                    |
// +--------------------------------------------------------------+
void PlatSappInit(void)
{
	TracyCZoneN(_funcZone, "PlatSappInit", true);
	ScratchBegin(scratch);
	ScratchBegin1(scratch2, scratch);
	ScratchBegin2(scratch3, scratch, scratch2);
	
	InitAppInput(&platformData->appInputs[0]);
	InitAppInput(&platformData->appInputs[1]);
	platformData->currentAppInput = &platformData->appInputs[0];
	platformData->oldAppInput = &platformData->appInputs[1];
	
	platformInfo = AllocType(PlatformInfo, stdHeap);
	NotNull(platformInfo);
	ClearPointer(platformInfo);
	platformInfo->platformStdHeap = stdHeap;
	platformInfo->platformStdHeapAllowFreeWithoutSize = &platformData->stdHeapAllowFreeWithoutSize;
	platformInfo->programArgs = &programArgs;
	
	platform = AllocType(PlatformApi, stdHeap);
	NotNull(platform);
	ClearPointer(platform);
	platform->GetNativeWindowHandle = Plat_GetNativeWindowHandle;
	platform->RequestQuit = Plat_RequestQuit;
	platform->GetSokolSwapchain = Plat_GetSokolSwapchain;
	platform->SetMouseLocked = Plat_SetMouseLocked;
	platform->SetMouseCursorType = Plat_SetMouseCursorType;
	platform->SetWindowTitle = Plat_SetWindowTitle;
	platform->SetWindowIcon = Plat_SetWindowIcon;
	platform->SetWindowTopmost = Plat_SetWindowTopmost;
	
	#if BUILD_INTO_SINGLE_UNIT
	{
		WriteLine_N("Compiled as single unit!");
		platformData->appApi = AppGetApi();
	}
	#else
	{
		#if TARGET_IS_WINDOWS
		platformData->appDllPath = StrLit(PROJECT_DLL_NAME_STR ".dll");
		platformData->appDllTempPath = StrLit(PROJECT_DLL_NAME_STR "_TEMP.dll");
		#elif TARGET_IS_LINUX
		platformData->appDllPath = StrLit("./" PROJECT_DLL_NAME_STR ".so");
		platformData->appDllTempPath = StrLit("./" PROJECT_DLL_NAME_STR "_TEMP.so");
		#else
		#error Current TARGET doesn't have an implementation for shared library suffix!
		#endif
		bool copyResult = OsCopyFile(platformData->appDllPath, platformData->appDllTempPath);
		Assert(copyResult == true);
		Result loadDllResult = OsLoadDll(platformData->appDllTempPath, &platformData->appDll);
		if (loadDllResult != Result_Success) { PrintLine_E("Failed to load \"%.*s\": %s", StrPrint(platformData->appDllPath), GetResultStr(loadDllResult)); }
		Assert(loadDllResult == Result_Success);
		OsInitFileWatch(stdHeap, platformData->appDllPath, 0, 0, &platformData->appDllWatch);
		
		AppGetApi_f* appGetApi = (AppGetApi_f*)OsFindDllFunc(&platformData->appDll, StrLit("AppGetApi"));
		NotNull(appGetApi);
		platformData->appApi = appGetApi();
		NotNull(platformData->appApi.AppInit);
		NotNull(platformData->appApi.AppUpdate);
		NotNull(platformData->appApi.AppClosing);
		NotNull(platformData->appApi.AppBeforeReload);
		NotNull(platformData->appApi.AppAfterReload);
	}
	#endif
	
	//TODO: Should we do an early call into app dll to get options?
	
	InitSokolGraphics((sg_desc){
		// .buffer_pool_size = ?; //int
		// .image_pool_size = ?; //int
		// .sampler_pool_size = ?; //int
		// .shader_pool_size = ?; //int
		// .pipeline_pool_size = ?; //int
		// .attachments_pool_size = ?; //int
		// .uniform_buffer_size = ?; //int
		// .max_commit_listeners = ?; //int
		// .disable_validation = ?; //bool    // disable validation layer even in debug mode, useful for tests
		// .d3d11_shader_debugging = ?; //bool    // if true, HLSL shaders are compiled with D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
		// .mtl_force_managed_storage_mode = ?; //bool // for debugging: use Metal managed storage mode for resources even with UMA
		// .mtl_use_command_buffer_with_retained_references = ?; //bool    // Metal: use a managed MTLCommandBuffer which ref-counts used resources
		// .wgpu_disable_bindgroups_cache = ?; //bool  // set to true to disable the WebGPU backend BindGroup cache
		// .wgpu_bindgroups_cache_size = ?; //int      // number of slots in the WebGPU bindgroup cache (must be 2^N)
		// .allocator = ?; //sg_allocator TODO: Fill this out!
		.environment = CreateSokolAppEnvironment(),
		.logger.func = SokolLogCallback,
	});
	InitGfxSystem(stdHeap, &gfx);
	#if DEBUG_BUILD
	gfx.prevFontFlow.numGlyphsAlloc = 256;
	gfx.prevFontFlow.glyphs = AllocArray(FontFlowGlyph, stdHeap, gfx.prevFontFlow.numGlyphsAlloc);
	NotNull(gfx.prevFontFlow.glyphs);
	#endif
	
	bool topmostFlagValue = FindNamedProgramArgBoolEx(&programArgs, StrLit("top"), StrLit("topmost"), false, 0);
	#if (false && TARGET_IS_WINDOWS)
	Plat_SetWindowTopmost(topmostFlagValue);
	#else
	//TODO: This notification comes too early, before the application has the GlobalNotificationQueue registered. So it doesn't show up visually. Maybe we should parse this argument later?
	if (topmostFlagValue == true) { Notify_W("Topmost is not available on the current platform! Ignoring command-line argument!"); }
	#endif
	
	platformData->appMemoryPntr = platformData->appApi.AppInit(platformInfo, platform);
	NotNull(platformData->appMemoryPntr);
	
	ScratchEnd(scratch3);
	ScratchEnd(scratch2);
	ScratchEnd(scratch);
	OsMarkStartTime();
	TracyCZoneEnd(_funcZone);
}

#if BUILD_WITH_SOKOL_APP

void PlatSappCleanup(void)
{
	platformData->appApi.AppClosing(platformInfo, platform, platformData->appMemoryPntr);
	ShutdownSokolGraphics();
}

void PlatSappEvent(const sapp_event* event)
{
	TracyCZoneN(_funcZone, "PlatSappEvent", true);
	bool handledEvent = false;
	
	if (platformData->currentAppInput != nullptr)
	{
		handledEvent = HandleSokolKeyboardMouseAndTouchEvents(
			event,
			platformData->currentAppInput->programTime, //TODO: Calculate a more accurate programTime to pass here!
			MakeV2i((i32)sapp_width(), (i32)sapp_height()),
			&platformData->currentAppInput->keyboard,
			&platformData->currentAppInput->mouse,
			nullptr, //TODO: Add touch support?
			sapp_mouse_locked()
		);
	}
	
	if (!handledEvent)
	{
		switch (event->type)
		{
			case SAPP_EVENTTYPE_TOUCHES_BEGAN:     WriteLine_D("Event: TOUCHES_BEGAN");     break;
			case SAPP_EVENTTYPE_TOUCHES_MOVED:     WriteLine_D("Event: TOUCHES_MOVED");     break;
			case SAPP_EVENTTYPE_TOUCHES_ENDED:     WriteLine_D("Event: TOUCHES_ENDED");     break;
			case SAPP_EVENTTYPE_TOUCHES_CANCELLED: WriteLine_D("Event: TOUCHES_CANCELLED"); break;
			case SAPP_EVENTTYPE_RESIZED:           /* PrintLine_D("Event: RESIZED %dx%d / %dx%d on thread %u", event->window_width, event->window_height, event->framebuffer_width, event->framebuffer_height, GetCurrentThreadId()); */ break;
			case SAPP_EVENTTYPE_ICONIFIED:
			{
				if (platformData->currentAppInput != nullptr && platformData->currentAppInput->isMinimized == false)
				{
					platformData->currentAppInput->isMinimized = true;
					platformData->currentAppInput->isMinimizedChanged = true;
				}
			} break;
			case SAPP_EVENTTYPE_RESTORED:
			{
				if (platformData->currentAppInput != nullptr && platformData->currentAppInput->isMinimized == true)
				{
					platformData->currentAppInput->isMinimized = false;
					platformData->currentAppInput->isMinimizedChanged = true;
				}
			} break;
			case SAPP_EVENTTYPE_FOCUSED:
			{
				if (platformData->currentAppInput != nullptr && platformData->currentAppInput->isFocused == false)
				{
					platformData->currentAppInput->isFocused = true;
					platformData->currentAppInput->isFocusedChanged = true;
				}
			} break;
			case SAPP_EVENTTYPE_UNFOCUSED:
			{
				if (platformData->currentAppInput != nullptr && platformData->currentAppInput->isFocused == true)
				{
					platformData->currentAppInput->isFocused = false;
					platformData->currentAppInput->isFocusedChanged = true;
				}
			} break;
			case SAPP_EVENTTYPE_SUSPENDED:         WriteLine_D("Event: SUSPENDED");         break;
			case SAPP_EVENTTYPE_RESUMED:           WriteLine_D("Event: RESUMED");           break;
			case SAPP_EVENTTYPE_QUIT_REQUESTED:    WriteLine_D("Event: QUIT_REQUESTED");    break;
			case SAPP_EVENTTYPE_CLIPBOARD_PASTED:  WriteLine_D("Event: CLIPBOARD_PASTED");  break;
			case SAPP_EVENTTYPE_FILES_DROPPED:
			{
				int numDroppedFiles = sapp_get_num_dropped_files();
				Assert(numDroppedFiles > 0);
				Str8* newDroppedFilePaths = VarArrayAddMulti(Str8, &platformData->currentAppInput->droppedFilePaths, (uxx)numDroppedFiles);
				for (uxx fIndex = 0; fIndex < (uxx)numDroppedFiles; fIndex++)
				{
					const char* filePathPntr = sapp_get_dropped_file_path((int)fIndex);
					NotNull(filePathPntr);
					newDroppedFilePaths[fIndex] = AllocStr8(stdHeap, MakeStr8Nt(filePathPntr));
				}
			}break;
			
			//NOTE: We currently only get this event when using OpenGL as the rendering backend since D3D11 has weird problems when we try to resize/render inside the WM_PAINT event
			#if TARGET_IS_WINDOWS
			//NOTE: I added this event type in order to update/render while the app is resized on Windows
			case SAPP_EVENTTYPE_RESIZE_RENDER:
			{
				PlatDoUpdate();
				sapp_consume_event(); //This tells Sokol backend that we actually rendered and want a frame flip
			} break;
			#endif //TARGET_IS_WINDOWS
			
			default: PrintLine_D("Event: UNKNOWN(%d)", event->type); break;
		}
	}
	TracyCZoneEnd(_funcZone);
}

sapp_desc sokol_main(int argc, char* argv[])
{
	#if PROFILING_ENABLED
	TracyCSetThreadName("main");
	Str8 projectName = StrLit(PROJECT_READABLE_NAME_STR);
	TracyCAppInfo(projectName.chars, projectName.length);
	#endif
	
	OsMarkStartTime(); //NOTE: We reset this at the end of PlatSappInit
	
	InitDebugOutputRouter(nullptr);
	
	Arena stdHeapLocal = ZEROED;
	InitArenaStdHeap(&stdHeapLocal);
	// FlagSet(stdHeapLocal.flags, ArenaFlag_AddPaddingForDebug);
	platformData = AllocType(PlatformData, &stdHeapLocal);
	NotNull(platformData);
	ClearPointer(platformData);
	MyMemCopy(&platformData->stdHeap, &stdHeapLocal, sizeof(Arena));
	stdHeap = &platformData->stdHeap;
	InitArenaStdHeap(&platformData->stdHeapAllowFreeWithoutSize);
	FlagSet(platformData->stdHeapAllowFreeWithoutSize.flags, ArenaFlag_AllowFreeWithoutSize);
	// FlagSet(platformData->stdHeapAllowFreeWithoutSize.flags, ArenaFlag_AddPaddingForDebug);
	
	#if 0
	char printBuffer[256];
	BufferPrintLine_D(printBuffer, "There %s %llu program argument%s", PluralEx(argc, "is", "are"), (uxx)argc, Plural(argc, "s"));
	for (uxx aIndex = 0; aIndex < (uxx)argc; aIndex++)
	{
		BufferPrintLine_D(printBuffer, "Arg[%llu]: \"%s\"", aIndex, argv[aIndex]);
	}
	#endif
	
	//TODO: Is the below true for other platforms??
	Assert(argc >= 1); //First argument on Windows and Linux (Ubuntu tested) is always the path to our executable
	//NOTE: We are getting a warning in clang unless we make an explicit cast: warning: passing 'char **' to parameter of type 'const char **' discards qualifiers in nested pointer types
	ParseProgramArgs(stdHeap, (uxx)argc-1, (const char**)&argv[1], &programArgs);
	
	v2 windowSize = DEFAULT_WINDOW_SIZE;
	Str8 sizeStr = FindNamedProgramArgStr(&programArgs, StrLit("size"), StrLit("s"), Str8_Empty);
	if (!IsEmptyStr(sizeStr))
	{
		v2 newSize = V2_Zero;
		if (TryParseV2(sizeStr, &newSize, nullptr))
		{
			windowSize = newSize;
		}
	}
	if (windowSize.Width < MIN_WINDOW_SIZE.Width) { windowSize.Width = MIN_WINDOW_SIZE.Width; }
	if (windowSize.Height < MIN_WINDOW_SIZE.Height) { windowSize.Height = MIN_WINDOW_SIZE.Height; }
	
	InitScratchArenasVirtual(Gigabytes(4));
	
	return (sapp_desc){
		.init_cb = PlatSappInit,
		.frame_cb = PlatDoUpdate,
		.cleanup_cb = PlatSappCleanup,
		.event_cb = PlatSappEvent,
		.width = RoundR32i(windowSize.Width),
		.height = RoundR32i(windowSize.Height),
		.swap_interval = 1, //16ms aka 60fps
		.window_title = "Loading...",
		.icon.sokol_default = false,
		.logger.func = SokolLogCallback,
		.enable_dragndrop = true,
		.max_dropped_files = 1,
	};
}

#endif

#if BUILD_INTO_SINGLE_UNIT
#include "app/app_main.c"
#endif
