/*
File:   build_config.h
Author: Taylor Robbins
Date:   01\19\2025
Description:
	** This file contains a bunch of options that control the build.bat.
	** This file is both a C header file that can be #included from a .c file,
	** and it is also scraped by the build.bat to extract values to change the work it performs.
	** Because it is scraped, and not parsed with the full C language spec, we must
	** be careful to keep this file very simple and not introduce any syntax that
	** would confuse the scraper when it's searching for values
*/

#ifndef _BUILD_CONFIG_H
#define /*DONT SHOW IN CSWITCH*/ _BUILD_CONFIG_H

#define BUILD_PIG_BUILD 0

// Controls whether we are making a build that we want to run with a Debugger.
// This often sacrifices runtime speed or code size for extra debug information.
// Debug builds often take less time to compile as well.
#define DEBUG_BUILD  1


// Build .exe binaries for Windows platform
#define BUILD_WINDOWS 0
// Build binaries for Linux platform(s)
#define BUILD_LINUX   1


// This disables hot-reloading support, the platform and game are one unit. Also PigCore gets compiled in directly rather than being used as a dynamic library
#define BUILD_INTO_SINGLE_UNIT  1
// Runs the sokol-shdc.exe on all .glsl files in the source directory to produce .glsl.h and .glsl.c files and then compiles the .glsl.c files to .obj
#define BUILD_SHADERS 0

// Builds dll_main.c into pig_core.dll and pig_core.lib
#define BUILD_PIG_CORE_DLL      0
// Compiles app/platform_main.c to %PROJECT_EXE_NAME%.exe
#define BUILD_APP_EXE  1
// Compiles app/app_main.c to %PROJECT_DLL_NAME%.dll
#define BUILD_APP_DLL  0

// Compiles piggen/main.c
#define BUILD_PIGGEN   0


// This puts all the contents of _data/resources into a zip file and converts the contents of that zip into resources_zip.c (and resources_zip.h in app/)
#define BUNDLE_RESOURCES_ZIP    0
// The .exe will use the resources_zip.h/c file instead of loading resources from disk
#define USE_BUNDLED_RESOURCES   0


// Builds third_party/tracy/TracyClient.cpp in C++ mode into tracy.obj which will be linked into pig_core.dll
#define BUILD_TRACY_DLL 0
// Enables linking with tracy.lib to enable profiling through Tracy
#define PROFILING_ENABLED       0




// Copies the exe and dlls to the _data folder so they can be run alongside the resources folder more easily
// Our debugger projects usually run the exe from the _build folder but with working directory set to the _data folder
#define COPY_TO_DATA_DIRECTORY 1

// Rather than compiling the project(s) it will simply output the
// result of the preprocessor's pass over the code to the build folder
#define DUMP_PREPROCESSOR   0
// Generates assembly listing files for all compilation units
#define DUMP_ASSEMBLY       0

// Tells the sokol_gfx.h include to use OpenGL instead of D3D11 on Windows (NOTE: Smooth resizing only works in OpenGL mode right now!)
#define PREFER_OPENGL_OVER_D3D11 1



// Generates code using piggen.exe (you can turn this off if you're not making changes to generated code and you've already generated it once)
#define RUN_PIGGEN    0
// Runs the %PROJECT_EXE_NAME%.exe
#define RUN_APP        0



#define PROJECT_READABLE_NAME C-Switch
#define PROJECT_FOLDER_NAME   C_Switch
#define PROJECT_DLL_NAME      cswitch_app
#define PROJECT_EXE_NAME      cswitch

#ifndef STRINGIFY_DEFINE
#define STRINGIFY_DEFINE(define) STRINGIFY(define)
#endif
#ifndef STRINGIFY
#define STRINGIFY(text)          #text
#endif
#define PROJECT_READABLE_NAME_STR  STRINGIFY_DEFINE(PROJECT_READABLE_NAME)
#define PROJECT_FOLDER_NAME_STR    STRINGIFY_DEFINE(PROJECT_FOLDER_NAME)
#define PROJECT_DLL_NAME_STR       STRINGIFY_DEFINE(PROJECT_DLL_NAME)
#define PROJECT_EXE_NAME_STR       STRINGIFY_DEFINE(PROJECT_EXE_NAME)



//These are all expected to be constant for the duration of the project
#define BUILD_WITH_SOKOL_GFX  1
#define BUILD_WITH_SOKOL_APP  1
#define BUILD_WITH_CLAY       1
#define BUILD_WITH_RAYLIB     0
#define BUILD_WITH_BOX2D      0
#define BUILD_WITH_SDL        0
#define BUILD_WITH_OPENVR     0
#define BUILD_WITH_IMGUI      0
#define BUILD_WITH_PHYSX      0
#define BUILD_WITH_HTTP       0
#define BUILD_WITH_FREETYPE   1
#define BUILD_WITH_GTK        0

#define PIG_CORE_NUM_STANDARD_PEOPLE_NAMES 1000
#define PIG_CORE_NUM_STANDARD_ROCK_NAMES 100

#define DEBUG_OUTPUT_PRINT_THREAD_PREFIX 0

#endif //  _BUILD_CONFIG_H
