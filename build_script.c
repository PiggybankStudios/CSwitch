/*
File:   tools_pig_build_main.c
Author: Taylor Robbins
Date:   06\16\2025
Description: 
	** This is our C based build script that handles invoking the
	** compiler and other CLI tools to build the rest of the repository.
	** This tool must be easy to compile and must be entirely self-contained
	** This tool will open the build_config.h and scrape it at when
	** ran and decide what to build based on whats set in there.
	** We don't directly #include build_config.h because we don't want
	** to-recompile this tool every time one of the options changes
	** NOTE: We don't really spend any time freeing things in this program
	** since it's lifespan is short and it's memory requirements are tiny
*/

#define PIG_BUILD_PRINT_SYS_CMDS 0
#define PIG_BUILD_INCLUDE_OPTIONAL_HEADERS 1
#include "pig_build.h"

#if 0
//NOTE: We use miniz.h when BUNDLE_RESOURCES_ZIP is enabled
#define MINIZ_NO_STDIO //to disable all usage and any functions which rely on stdio for file I/O.
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
#define MINIZ_LITTLE_ENDIAN                  1
#include "core/third_party/miniz/miniz.h"
#include "core/third_party/miniz/miniz.c"
#endif

#define BUILD_CONFIG_PATH       "../build_config.h"

#define FOLDERNAME_GENERATED_CODE  "gen"
#define FOLDERNAME_LINUX           "linux"
#define FOLDERNAME_OSX             "osx"

#define FILENAME_RESOURCES_ZIP     "resources.zip"
#define FILENAME_WIN_RESOURCES_RES "resources.res"
#define FILENAME_PIGGEN_EXE        "piggen.exe"
#define FILENAME_PIGGEN            "piggen"
#define FILENAME_TRACY_DLL         "tracy.dll"
#define FILENAME_TRACY_LIB         "tracy.lib"
#define FILENAME_TRACY_SO          "tracy.so"
#define FILENAME_PIG_CORE_DLL      "pig_core.dll"
#define FILENAME_PIG_CORE_LIB      "pig_core.lib"
#define FILENAME_PIG_CORE_SO       "libpig_core.so"

#if BUILDING_ON_WINDOWS
#define TOOL_EXE_NAME      "pig_build.exe"
#else
#define TOOL_EXE_NAME      "pig_build"
#endif

#define T_SHADER_OBJS      "|ShaderObjs"

void PrintUsage()
{
	WriteLine_E("Usage: " BUILD_SCRIPT_EXE_NAME " [DEBUG_BUILD={1/0}] [BUILD_TESTS={1/0}] ...");
}

typedef struct BundleResourcesContext BundleResourcesContext;
struct BundleResourcesContext
{
	mz_zip_archive zip;
	Str relativePath;
	StrArray resourcePaths;
	u64 uncompressedSize;
	u64 archiveAllocSize;
	u64 archiveSize;
	u8* archivePntr;
};

// +==============================+
// |   BundleResourcesCallback    |
// +==============================+
// bool BundleResourcesCallback(Str path, bool isFolder, void* contextPntr)
RECURSIVE_DIR_WALK_CALLBACK_DEF(BundleResourcesCallback)
{
	BundleResourcesContext* context = (BundleResourcesContext*)contextPntr;
	if (!isFolder)
	{
		Str fileContents = ReadEntireFile(path);
		assert(StrExactStartsWith(path, context->relativePath));
		Str inZipPath = StrSliceFrom(path, context->relativePath.length);
		if (inZipPath.length > 0 && IsSlash(inZipPath.chars[0])) { inZipPath.length--; inZipPath.chars++; }
		Str inZipPathNt = CopyStr(inZipPath);
		FixPathSlashes(inZipPathNt, '/');
		mz_bool addMemSuccess = mz_zip_writer_add_mem(&context->zip, inZipPathNt.chars, fileContents.bytes, (size_t)fileContents.length, (mz_uint)MZ_BEST_COMPRESSION);
		assert(addMemSuccess == MZ_TRUE);
		context->uncompressedSize += fileContents.length;
		AddStr(&context->resourcePaths, inZipPath);
		free(inZipPathNt.chars);
		free(fileContents.chars);
	}
	return true;
}
size_t ZipFileWriteCallback(void* contextPntr, mz_uint64 fileOffset, const void* bufferPntr, size_t numBytes)
{
	// PrintLine("ZipFileWriteCallback(%p, %llu, %p, %zu)", contextPntr, fileOffset, bufferPntr, numBytes);
	BundleResourcesContext* context = (BundleResourcesContext*)contextPntr;
	assert(context != nullptr);
	if (context->archiveAllocSize < fileOffset + numBytes)
	{
		u64 newAllocSize = context->archiveAllocSize;
		if (newAllocSize < 8) { newAllocSize = 8; }
		while (newAllocSize < fileOffset + numBytes) { newAllocSize *= 2; }
		void* newAllocPntr = malloc(newAllocSize);
		if (context->archiveSize > 0) { memcpy(newAllocPntr, context->archivePntr, context->archiveSize); }
		if (context->archivePntr != nullptr) { free(context->archivePntr); }
		context->archivePntr = newAllocPntr;
		context->archiveAllocSize = newAllocSize;
	}
	memcpy(&context->archivePntr[fileOffset], bufferPntr, numBytes);
	if (context->archiveSize < fileOffset + numBytes) { context->archiveSize = fileOffset + numBytes; }
	return numBytes;
}

int main(int argc, char* argv[])
{
	RecompileIfNeeded(StrArray_Empty);
	PrintLine("[%s...]", BUILD_SCRIPT_EXE_NAME);
	bool isMsvcInitialized = WasMsvcDevBatchRun();
	StrArray commonTags = EMPTY;
	
	// +==============================+
	// |       Extract Defines        |
	// +==============================+
	Str buildConfigContents = ReadEntireFile(StrLit(BUILD_CONFIG_PATH));
	
	Str PROJECT_DLL_NAME  = CopyStr(ExtractStrDefine(buildConfigContents, StrLit("PROJECT_DLL_NAME")));
	Str PROJECT_EXE_NAME  = CopyStr(ExtractStrDefine(buildConfigContents, StrLit("PROJECT_EXE_NAME")));
	// Str PROJECT_READABLE_NAME = ExtractStrDefine(buildConfigContents, StrLit("PROJECT_READABLE_NAME"));
	// Str PROJECT_FOLDER_NAME = ExtractStrDefine(buildConfigContents, StrLit("PROJECT_FOLDER_NAME"));
	
	#define LOAD_CONFIG(CONFIG_NAME)                                                     \
		bool CONFIG_NAME = ExtractBoolDefine(buildConfigContents, StrLit(#CONFIG_NAME)); \
		if (CONFIG_NAME) { AddStrLit(&commonTags, #CONFIG_NAME); }                       \
		do {} while(0)
	LOAD_CONFIG(DEBUG_BUILD);
	LOAD_CONFIG(BUILD_INTO_SINGLE_UNIT);
	LOAD_CONFIG(USE_BUNDLED_RESOURCES);
	LOAD_CONFIG(BUILD_WINDOWS);
	LOAD_CONFIG(BUILD_LINUX);
	LOAD_CONFIG(BUILD_OSX);
	LOAD_CONFIG(BUILD_SHADERS);
	LOAD_CONFIG(BUILD_PIGGEN);
	LOAD_CONFIG(RUN_PIGGEN);
	LOAD_CONFIG(BUILD_TRACY_DLL);
	LOAD_CONFIG(PROFILING_ENABLED);
	LOAD_CONFIG(BUNDLE_RESOURCES_ZIP);
	LOAD_CONFIG(BUILD_PIG_CORE_DLL);
	LOAD_CONFIG(BUILD_APP_EXE);
	LOAD_CONFIG(BUILD_APP_DLL);
	LOAD_CONFIG(RUN_APP);
	LOAD_CONFIG(COPY_TO_DATA_DIRECTORY);
	LOAD_CONFIG(DUMP_PREPROCESSOR);
	LOAD_CONFIG(DUMP_ASSEMBLY);
	LOAD_CONFIG(BUILD_WITH_SOKOL_GFX);
	LOAD_CONFIG(BUILD_WITH_SOKOL_APP);
	LOAD_CONFIG(BUILD_WITH_FREETYPE);
	LOAD_CONFIG(BUILD_WITH_GTK);
	#undef LOAD_CONFIG
	
	free(buildConfigContents.chars);
	
	Str filenameAppDll    = JoinStrings2(PROJECT_DLL_NAME, StrLit(".dll"));
	Str filenameAppDllAsm = JoinStrings2(PROJECT_DLL_NAME, StrLit(".asm"));
	Str filenameAppSo     = JoinStrings2(PROJECT_DLL_NAME, StrLit(".so"));
	Str filenameAppDylib  = JoinStrings2(PROJECT_DLL_NAME, StrLit(".dylib"));
	Str filenameAppExe    = JoinStrings2(PROJECT_EXE_NAME, StrLit(".exe"));
	Str filenameAppAsm    = JoinStrings2(PROJECT_EXE_NAME, StrLit(".asm"));
	Str filenameApp       = PROJECT_EXE_NAME;
	
	// +==============================+
	// | Parse Command-Line Arguments |
	// +==============================+
	if (argc > 1)
	{
		PrintLine("Got %d argument%s", argc-1, (argc-1 == 1) ? "" : "s");
		for (int aIndex = 1; aIndex < argc; aIndex++)
		{
			PrintLine("Arg[%d]: %s", aIndex-1, argv[aIndex]);
			//TODO: We should parse these arguments and use them as overrides to the #defines we loaded above!
		}
		PrintUsage(); //TODO: Only print this out if we find an argument we don't understand
		PrintLine_E("ERROR: Command-line arguments are not supported yet!");
		return 1;
	}
	
	// +==============================+
	// | Enforce Option Restrictions  |
	// +==============================+
	if (BUILD_WINDOWS && !BUILDING_ON_WINDOWS)
	{
		WriteLine_E("BUILD_WINDOWS does not working when building on non-Windows platforms");
		BUILD_WINDOWS = false;
	}
	if (BUILD_OSX && !BUILDING_ON_OSX)
	{
		WriteLine_E("BUILD_OSX does not working when building on non-Mac platforms");
		BUILD_OSX = false;
	}
	if (BUILD_LINUX && !BUILDING_ON_LINUX && !BUILDING_ON_WINDOWS)
	{
		WriteLine_E("BUILD_LINUX only works when building on Linux (or on Windows through WSL)");
		BUILD_LINUX = false;
	}
	if (BUILD_INTO_SINGLE_UNIT && BUILD_APP_DLL && !BUILD_APP_EXE)
	{
		WriteLine_E("BUILD_INTO_SINGLE_UNIT works with BUILD_APP_EXE but only BUILD_APP_DLL is enabled. Assuming we want BUILD_APP_EXE instead");
		BUILD_APP_DLL = false;
		BUILD_APP_EXE = true;
	}
	if (BUILD_INTO_SINGLE_UNIT && BUILD_APP_DLL)
	{
		WriteLine_E("BUILD_INTO_SINGLE_UNIT implies that BUILD_APP_DLL is unnecassary. Only BUILD_APP_EXE matters");
		BUILD_APP_DLL = false;
	}
	if (BUILD_INTO_SINGLE_UNIT && BUILD_APP_EXE && BUILD_PIG_CORE_DLL)
	{
		WriteLine_E("BUILD_INTO_SINGLE_UNIT implies that BUILD_PIG_CORE_DLL is unnecassary. Not building pig_core.dll/so");
		BUILD_PIG_CORE_DLL = false;
	}
	
	// +==============================+
	// |      Fill Common Flags       |
	// +==============================+
	CliArgs commonCompilerFlags = EMPTY;
	CliArgs commonLinkerFlags = EMPTY;
	FillPigCoreFlags(&commonCompilerFlags, &commonLinkerFlags, StrLit("[ROOT]/core"));
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL, CL_INCLUDE_DIR, "[ROOT]/app");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL, CL_INCLUDE_DIR, "[ROOT]/core");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/app");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/core");
	
	//clang_AndroidFlags, clang_AndroidLinkFlags, clang_WasmFlags, clang_WebFlags, clang_OrcaFlags
	//cl_PlaydateSimulatorCompilerFlags, link_PlaydateSimulatorLinkerFlags, link_PlaydateSimulatorLibraries
	//gcc_PlaydateDeviceCommonFlags, gcc_PlaydateDeviceCompilerFlags, gcc_PlaydateDeviceLinkerFlags, pdc_CommonFlags
	
	// +--------------------------------------------------------------+
	// |                       Build piggen.exe                       |
	// +--------------------------------------------------------------+
	#if BUILDING_ON_WINDOWS
	#define RUNNABLE_FILENAME_PIGGEN FILENAME_PIGGEN_EXE
	#else
	#define RUNNABLE_FILENAME_PIGGEN FILENAME_PIGGEN
	#endif
	if (RUN_PIGGEN && !BUILD_PIGGEN && !DoesFileExist(StrLit(RUNNABLE_FILENAME_PIGGEN))) { PrintLine("Building %s because it's missing", RUNNABLE_FILENAME_PIGGEN); BUILD_PIGGEN = true; }
	if (BUILD_PIGGEN)
	{
		if (BUILD_WINDOWS)
		{
			InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized);
			PrintLine("\n[Building %s for Windows...]", FILENAME_PIGGEN_EXE);
			
			CliArgs cmd = EMPTY;
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/core/piggen/piggen_main.c");
			AddArgNt(&cmd, CL_BINARY_FILE, FILENAME_PIGGEN_EXE);
			AddArgList(&cmd, &commonCompilerFlags);
			if (DUMP_ASSEMBLY) { AddArgNt(&cmd, CL_ASSEMB_LISTING_FILE, "piggen.asm"); }
			AddArg(&cmd, CL_LINK);
			AddArgList(&cmd, &commonLinkerFlags);
			AddArgNt(&cmd, CLI_QUOTED_ARG, "Shlwapi.lib"); //Needed for PathFileExistsA
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_MSVC_CL);
			AddTag(&tags, T_WINDOWS);
			AddTag(&tags, T_LANG_C);
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, StrLit("Failed to build " FILENAME_PIGGEN_EXE "!"));
			AssertFileExist(StrLit(FILENAME_PIGGEN_EXE), true);
			PrintLine("[Built %s for Windows!]", FILENAME_PIGGEN_EXE);
		}
		if (BUILD_LINUX)
		{
			PrintLine("\n[Building %s for Linux...]", FILENAME_PIGGEN);
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/piggen/piggen_main.c");
			AddArgNt(&cmd, CLANG_OUTPUT_FILE, FILENAME_PIGGEN);
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_LINUX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_C);
			
			#if BUILDING_ON_LINUX
			Str clangExe = StrLit(EXE_CLANG);
			#else
			Str clangExe = StrLit(EXE_WSL_CLANG);
			mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
			chdir(FOLDERNAME_LINUX);
			cmd.rootDirPath = StrLit("../..");
			#endif
			
			RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, StrLit("Failed to build " FILENAME_PIGGEN "!"));
			AssertFileExist(StrLit(FILENAME_PIGGEN), true);
			PrintLine("[Built %s for Linux!]", FILENAME_PIGGEN);
			
			#if !BUILDING_ON_LINUX
			chdir("..");
			#endif
		}
		//TODO: Add OSX support
	}
	
	// +--------------------------------------------------------------+
	// |                        Run piggen.exe                        |
	// +--------------------------------------------------------------+
	if (RUN_PIGGEN)
	{
		PrintLine("\n[%s]", RUNNABLE_FILENAME_PIGGEN);
		
		#define PIGGEN_OUTPUT_FOLDER "-o=\"[VAL]\""
		#define PIGGEN_EXCLUDE_FOLDER "-e=\"[VAL]\""
		
		CliArgs cmd = EMPTY;
		AddArgNt(&cmd, CLI_QUOTED_ARG, "..");
		AddArgNt(&cmd, PIGGEN_OUTPUT_FOLDER, FOLDERNAME_GENERATED_CODE "/");
		
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/.git/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_build/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_data/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_media/");
		
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/base/base_defines_check.h");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/piggen/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/tools/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/third_party/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/wasm/std/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/.git/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_build/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_scripts/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_media/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_template/");
		AddArgNt(&cmd, PIGGEN_EXCLUDE_FOLDER, "[ROOT]/core/_fuzzing/");
		
		RunCliProgramAndExitOnFailure(StrLit(EXEC_PROGRAM_IN_FOLDER_PREFIX RUNNABLE_FILENAME_PIGGEN), &cmd, StrLit(RUNNABLE_FILENAME_PIGGEN " Failed!"));
	}
	
	// +--------------------------------------------------------------+
	// |                       Build tracy.dll                        |
	// +--------------------------------------------------------------+
	if (PROFILING_ENABLED && !BUILD_TRACY_DLL && BUILD_WINDOWS && !DoesFileExist(StrLit(FILENAME_TRACY_DLL))) { PrintLine("Building %s because it's missing", FILENAME_TRACY_DLL); BUILD_TRACY_DLL = true; }
	if (PROFILING_ENABLED && !BUILD_TRACY_DLL && BUILD_LINUX && !DoesFileExist(StrLit(FILENAME_TRACY_SO))) { PrintLine("Building %s because it's missing", FILENAME_TRACY_SO); BUILD_TRACY_DLL = true; }
	if (BUILD_TRACY_DLL)
	{
		if (BUILD_WINDOWS)
		{
			InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized);
			PrintLine("[Building %s for Windows...]", FILENAME_TRACY_DLL);
			
			CliArgs cmd = EMPTY;
			// AddArg(&cmd, CL_COMPILE);
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/core/third_party/tracy/TracyClient.cpp");
			AddArgNt(&cmd, CL_INCLUDE_DIR, "[ROOT]/core/third_party/tracy");
			AddArgNt(&cmd, CL_BINARY_FILE, FILENAME_TRACY_DLL);
			AddArgNt(&cmd, CL_DEFINE, "TRACY_ENABLE");
			AddArgNt(&cmd, CL_DEFINE, "TRACY_EXPORTS");
			AddArgNt(&cmd, CL_CONFIGURE_EXCEPTION_HANDLING, "s"); //enable stack-unwinding
			AddArgNt(&cmd, CL_CONFIGURE_EXCEPTION_HANDLING, "c"); //extern "C" functions don't through exceptions
			AddArgList(&cmd, &commonCompilerFlags);
			if (DUMP_ASSEMBLY) { AddArgNt(&cmd, CL_ASSEMB_LISTING_FILE, "tracy.asm"); }
			AddArg(&cmd, CL_LINK);
			AddArg(&cmd, LINK_BUILD_DLL);
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_MSVC_CL);
			AddTag(&tags, T_WINDOWS);
			AddTag(&tags, T_LANG_CPP);
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, StrLit("Failed to build " FILENAME_TRACY_DLL "!"));
			AssertFileExist(StrLit(FILENAME_TRACY_DLL), true);
			PrintLine("[Built %s for Windows!]", FILENAME_TRACY_DLL);
		}
		if (BUILD_LINUX)
		{
			PrintLine("\n[Building %s for Linux...]", FILENAME_TRACY_SO);
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/core/third_party/tracy/TracyClient.cpp");
			AddArgNt(&cmd, CLANG_INCLUDE_DIR, "[ROOT]/core/third_party/tracy");
			AddArgNt(&cmd, CLANG_OUTPUT_FILE, FILENAME_TRACY_SO);
			AddArg(&cmd, CLANG_BUILD_SHARED_LIB);
			AddArg(&cmd, CLANG_fPIC);
			AddArgNt(&cmd, CLANG_DEFINE, "TRACY_ENABLE");
			AddArgNt(&cmd, CLANG_DEFINE, "TRACY_EXPORTS");
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgList(&cmd, &commonLinkerFlags);
			AddArgNt(&cmd, CLANG_DISABLE_WARNING, CLANG_WARNING_SHADOWING); // declaration shadows a local variable
			AddArgNt(&cmd, CLANG_DISABLE_WARNING, CLANG_WARNING_MISSING_FIELD_INITIALIZERS); // missing field 'extra' initializer
			AddArgNt(&cmd, CLANG_DISABLE_WARNING, CLANG_WARNING_MISSING_FALLTHROUGH_IN_SWITCH); // unannotated fall-through between switch labels
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_LINUX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_CPP);
			
			#if BUILDING_ON_LINUX
			Str clangExe = StrLit(EXE_CLANG);
			#else
			Str clangExe = StrLit(EXE_WSL_CLANG);
			mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
			chdir(FOLDERNAME_LINUX);
			cmd.rootDirPath = StrLit("../..");
			#endif
			
			RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, StrLit("Failed to build " FILENAME_TRACY_SO "!"));
			AssertFileExist(StrLit(FILENAME_TRACY_SO), true);
			PrintLine("[Built %s for Linux!]", FILENAME_TRACY_SO);
			
			#if !BUILDING_ON_LINUX
			chdir("..");
			#endif
		}
		//TODO: Add OSX support
	}
	if (PROFILING_ENABLED)
	{
		AddTaggedArgNt(&commonLinkerFlags, T_MSVC_CL, CLI_QUOTED_ARG, FILENAME_TRACY_LIB);
		AddTaggedArgNt(&commonLinkerFlags, T_CLANG,   CLI_QUOTED_ARG, FILENAME_TRACY_SO);
	}
	
	// +--------------------------------------------------------------+
	// |                       Bundle Resources                       |
	// +--------------------------------------------------------------+
	if (BUNDLE_RESOURCES_ZIP)
	{
		#if 0
		//TODO: Move away from using python! This is the last script we depend on, currently
		CliArgs cmd = EMPTY;
		AddArgNt(&cmd, CLI_UNQUOTED_ARG, "[ROOT]/core/_scripts/pack_resources.py");
		AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/_data/resources");
		AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_RESOURCES_ZIP);
		AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/resources_zip.h");
		AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/resources_zip.c");
		
		RunCliProgramAndExitOnFailure(StrLit("python"), &cmd, StrLit("pack_resources.py Failed!"));
		AssertFileExist(StrLit(FILENAME_RESOURCES_ZIP), true);
		#else
		BundleResourcesContext context = EMPTY;
		context.zip.m_pWrite = ZipFileWriteCallback;
		context.zip.m_pIO_opaque = &context;
		mz_bool initResult = mz_zip_writer_init(&context.zip, 0);
		if (initResult != MZ_TRUE) { PrintLine_E("zip error: %s", mz_zip_get_error_string(context.zip.m_last_error)); }
		assert(initResult == MZ_TRUE);
		context.relativePath = StrLit("../_data/resources");
		RecursiveDirWalk(StrLit("../_data/resources"), BundleResourcesCallback, &context);
		mz_bool finalizeResult = mz_zip_writer_finalize_archive(&context.zip);
		assert(finalizeResult == MZ_TRUE);
		mz_zip_writer_end(&context.zip);
		PrintLine("Found %llu resource files, total %llu bytes uncompressed, %llu compressed (%.1f%%)", context.resourcePaths.length, context.uncompressedSize, context.archiveSize, ((float)context.archiveSize / (float)context.uncompressedSize) * 100.0);
		
		CreateAndWriteFile(StrLit("resources.zip"), MakeStr(context.archiveSize, context.archivePntr), false);
		
		//Create resources_zip.h
		{
			Str cFileContents = EMPTY;
			for (int pass = 0; pass < 2; pass++)
			{
				u64 fileSize = 0;
				
				TwoPassPrint(&cFileContents, &fileSize,
					"/*\n"
					"File:   resources_zip.h\n"
					"Author: WARNING: This file is generated by pig_build.exe! Any hand edits will be lost!\n"
					"*/\n\n"
					"#ifndef _RESOURCES_ZIP_H\n"
					"#define _RESOURCES_ZIP_H\n\n"
				);
				TwoPassPrint(&cFileContents, &fileSize, "u8 resources_zip_bytes[%u];\n\n", context.archiveSize);
				TwoPassPrint(&cFileContents, &fileSize, "#endif //_RESOURCES_ZIP_H\n");
				
				if (pass == 0)
				{
					cFileContents.length = fileSize;
					cFileContents.pntr = malloc(cFileContents.length+1);
					assert(cFileContents.pntr != nullptr);
				}
				else { assert(fileSize == cFileContents.length); cFileContents.chars[cFileContents.length] = '\0'; }
			}
			CreateAndWriteFile(StrLit("../app/resources_zip.h"), cFileContents, true);
			free(cFileContents.chars);
		}
		
		//Create resources_zip.c
		{
			Str cFileContents = EMPTY;
			for (int pass = 0; pass < 2; pass++)
			{
				u64 fileSize = 0;
				
				TwoPassPrint(&cFileContents, &fileSize, "// This file is generated by pig_build.exe! Any hand edits will be lost!\n\n");
				TwoPassPrint(&cFileContents, &fileSize, "// Archive Contents (%u file%s, %u bytes uncompressed):\n", context.resourcePaths.length, (context.resourcePaths.length == 1) ? "" : "s", context.uncompressedSize);
				for (u64 rIndex = 0; rIndex < context.resourcePaths.length; rIndex++)
				{
					TwoPassPrint(&cFileContents, &fileSize, "//\t%.*s\n", StrPrint(context.resourcePaths.strings[rIndex]));
				}
				TwoPassPrint(&cFileContents, &fileSize, "\nu8 resources_zip_bytes[%u] = {\n\t", context.archiveSize);
				for (u64 bIndex = 0; bIndex < context.archiveSize; bIndex++)
				{
					if (bIndex > 0) { TwoPassPrint(&cFileContents, &fileSize, ",%s", (bIndex%32) == 0 ? "\n\t" : " "); }
					TwoPassPrint(&cFileContents, &fileSize, "0x%02X", context.archivePntr[bIndex]);
				}
				TwoPassPrint(&cFileContents, &fileSize, "\n};\n");
				
				if (pass == 0)
				{
					cFileContents.length = fileSize;
					cFileContents.pntr = malloc(cFileContents.length+1);
					assert(cFileContents.pntr != nullptr);
				}
				else { assert(fileSize == cFileContents.length); cFileContents.chars[cFileContents.length] = '\0'; }
			}
			CreateAndWriteFile(StrLit("../app/resources_zip.c"), cFileContents, true);
			free(cFileContents.chars);
		}
		
		#endif
	}
	
	// +--------------------------------------------------------------+
	// |                        Build Shaders                         |
	// +--------------------------------------------------------------+
	FindShadersContext findContext = EMPTY;
	{
		//NOTE: No ignoreList needed in findContext
		RecursiveDirWalk(StrLit("../app"), FindShaderFilesCallback, &findContext);
		
		if (BUILD_WINDOWS)
		{
			for (u64 sIndex = 0; sIndex < findContext.objPaths.length; sIndex++)
			{
				Str objPath = findContext.objPaths.strings[sIndex];
				AddTaggedArgStr(&commonLinkerFlags, T_WINDOWS T_SHADER_OBJS, CLI_QUOTED_ARG, objPath);
				if (!DoesFileExist(objPath) && !BUILD_SHADERS) { PrintLine("Building shaders because \"%.*s\" is missing!", StrPrint(objPath)); BUILD_SHADERS = true; }
			}
		}
		if (BUILD_LINUX)
		{
			for (u64 sIndex = 0; sIndex < findContext.oPaths.length; sIndex++)
			{
				Str oPath = findContext.oPaths.strings[sIndex];
				AddTaggedArgStr(&commonLinkerFlags, T_NOT_WINDOWS T_SHADER_OBJS, CLI_QUOTED_ARG, oPath);
				Str oPathWithFolder = BUILDING_ON_LINUX ? CopyStr(oPath) : JoinPaths(StrLit(FOLDERNAME_LINUX), oPath);
				if (!DoesFileExist(oPathWithFolder) && !BUILD_SHADERS) { PrintLine("Building shaders because \"%.*s\" is missing!", StrPrint(oPathWithFolder)); BUILD_SHADERS = true; }
			}
		}
		if (BUILD_OSX)
		{
			for (u64 sIndex = 0; sIndex < findContext.oPaths.length; sIndex++)
			{
				Str oPath = findContext.oPaths.strings[sIndex];
				AddTaggedArgStr(&commonLinkerFlags, T_NOT_WINDOWS T_SHADER_OBJS, CLI_QUOTED_ARG, oPath);
				if (!DoesFileExist(oPath) && !BUILD_SHADERS) { PrintLine("Building shaders because \"%.*s\" is missing!", StrPrint(oPath)); BUILD_SHADERS = true; }
			}
		}
		
		if (!BUILD_SHADERS)
		{
			FreeStrArray(&findContext.shaderPaths);
			FreeStrArray(&findContext.headerPaths);
			FreeStrArray(&findContext.sourcePaths);
			FreeStrArray(&findContext.objPaths);
			FreeStrArray(&findContext.oPaths);
		}
	}
	
	if (BUILD_SHADERS)
	{
		if (BUILD_WINDOWS) { InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized); }
		
		PrintLine("Found %llu shader%s", findContext.shaderPaths.length, findContext.shaderPaths.length == 1 ? "" : "s");
		// for (u64 sIndex = 0; sIndex < findContext.shaderPaths.length; sIndex++)
		// {
		// 	PrintLine("Shader[%u]", sIndex);
		// 	PrintLine("\t\"%.*s\"", StrPrint(findContext.shaderPaths.strings[sIndex]));
		// 	PrintLine("\t\"%.*s\"", StrPrint(findContext.headerPaths.strings[sIndex]));
		// 	PrintLine("\t\"%.*s\"", StrPrint(findContext.sourcePaths.strings[sIndex]));
		// 	PrintLine("\t\"%.*s\"", StrPrint(findContext.objPaths.strings[sIndex]));
		// 	PrintLine("\t\"%.*s\"", StrPrint(findContext.oPaths.strings[sIndex]));
		// }
		
		// First use shdc.exe to generate header files for each .glsl file
		for (u64 sIndex = 0; sIndex < findContext.shaderPaths.length; sIndex++)
		{
			Str shaderPath = findContext.shaderPaths.strings[sIndex];
			Str headerPath = findContext.headerPaths.strings[sIndex];
			Str realHeaderPath = ResolveRootTo(headerPath, StrLit(".."));
			Str realShaderPath = ResolveRootTo(shaderPath, StrLit(".."));
			
			CliArgs cmd = EMPTY;
			AddArgNt(&cmd, SHDC_FORMAT, "sokol_impl");
			AddArgNt(&cmd, SHDC_ERROR_FORMAT, "msvc");
			// AddArg(&cmd, SHDC_REFLECTION);
			AddArgNt(&cmd, SHDC_SHADER_LANGUAGES, "hlsl5:glsl430:metal_macos");
			AddArgStr(&cmd, SHDC_INPUT, shaderPath);
			AddArgStr(&cmd, SHDC_OUTPUT, headerPath);
			
			PrintLine("Generating \"%.*s\"...", StrPrint(realHeaderPath));
			Str shdcExe = JoinPaths(StrLit("../core"), StrLit(EXE_SHDC));
			FixPathSlashes(shdcExe, PATH_SEP_CHAR);
			RunCliProgramAndExitOnFailure(shdcExe, &cmd, StrLit(EXE_SHDC_NAME " failed on TODO:!"));
			AssertFileExist(realHeaderPath, true);
			
			ScrapeShaderHeaderFileAndAddExtraInfo(realHeaderPath, realShaderPath);
			free(realHeaderPath.chars);
			free(realShaderPath.chars);
		}
		
		//Then compile each header file to an .o/.obj file
		for (u64 sIndex = 0; sIndex < findContext.shaderPaths.length; sIndex++)
		{
			Str headerPath = findContext.headerPaths.strings[sIndex];
			Str sourcePath = findContext.sourcePaths.strings[sIndex];
			Str headerFileName = GetFileNamePart(headerPath, true);
			Str headerDirectory = GetDirectoryPart(headerPath, true);
			Str realSourcePath = ResolveRootTo(sourcePath, StrLit(".."));
			
			//We need a .c file that #includes shader_include.h (which defines SOKOL_SHDC_IMPL) and then the shader header file
			Str sourceFileContents = JoinStrings3(
				StrLit("\n#include \"shader_include.h\"\n\n#include \""),
				headerFileName,
				StrLit("\"\n")
			);
			PrintLine("Generating \"%.*s\"...", StrPrint(realSourcePath));
			CreateAndWriteFile(realSourcePath, sourceFileContents, true);
			
			if (BUILD_WINDOWS)
			{
				Str objPath = findContext.objPaths.strings[sIndex];
				
				CliArgs cmd = EMPTY;
				AddArg(&cmd, CL_COMPILE);
				AddArgStr(&cmd, CLI_QUOTED_ARG, sourcePath);
				AddArgStr(&cmd, CL_OBJ_FILE, objPath);
				AddArgStr(&cmd, CL_INCLUDE_DIR, headerDirectory);
				AddArgList(&cmd, &commonCompilerFlags);
				
				StrArray tags = EMPTY;
				AddStrArray(&tags, &commonTags);
				AddTag(&tags, T_MSVC_CL);
				AddTag(&tags, T_WINDOWS);
				AddTag(&tags, T_LANG_C);
				
				RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, FormatStr("Failed to build %.*s for Windows!", StrPrint(sourcePath)));
				AssertFileExist(objPath, true);
			}
			if (BUILD_LINUX)
			{
				Str oPath = findContext.oPaths.strings[sIndex];
				//TODO: The path we store in the findContext needs to have [ROOT] at the beginning somehow so we can get rid of this logic
				// Str fixedSourcePath = BUILDING_ON_LINUX ? CopyStr(sourcePath) : JoinStrings2(StrLit("../"), sourcePath, false);
				// FixPathSlashes(fixedSourcePath, '/');
				// Str fixedHeaderDirectory = BUILDING_ON_LINUX ? CopyStr(headerDirectory) : JoinStrings2(StrLit("../"), headerDirectory, false);
				// FixPathSlashes(fixedHeaderDirectory, '/');
				
				CliArgs cmd = EMPTY;
				cmd.pathSepChar = '/';
				AddArg(&cmd, CLANG_COMPILE);
				AddArgStr(&cmd, CLI_QUOTED_ARG, sourcePath);
				AddArgStr(&cmd, CLANG_OUTPUT_FILE, oPath);
				AddArgStr(&cmd, CLANG_INCLUDE_DIR, headerDirectory);
				AddArgNt(&cmd, CLANG_DISABLE_WARNING, "unused-command-line-argument"); //Clang likes to warn about _lib_debug/_lib_release library folder being unused
				AddArgList(&cmd, &commonCompilerFlags);
				
				StrArray tags = EMPTY;
				AddStrArray(&tags, &commonTags);
				AddTag(&tags, T_CLANG);
				AddTag(&tags, T_LINUX);
				AddTag(&tags, T_UNIX);
				AddTag(&tags, T_LANG_C);
				
				#if BUILDING_ON_LINUX
				Str clangExe = StrLit(EXE_CLANG);
				#else
				Str clangExe = StrLit(EXE_WSL_CLANG);
				mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
				chdir(FOLDERNAME_LINUX);
				cmd.rootDirPath = StrLit("../..");
				#endif
				
				RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, FormatStr("Failed to build %.*s for Linux!", StrPrint(sourcePath)));
				AssertFileExist(oPath, true);
				
				#if !BUILDING_ON_LINUX
				chdir("..");
				#endif
			}
			if (BUILD_OSX)
			{
				Str oPath = findContext.oPaths.strings[sIndex];
				
				CliArgs cmd = EMPTY;
				cmd.pathSepChar = '/';
				AddArg(&cmd, CLANG_COMPILE);
				AddArgStr(&cmd, CLI_QUOTED_ARG, sourcePath);
				AddArgStr(&cmd, CLANG_OUTPUT_FILE, oPath);
				AddArgStr(&cmd, CLANG_INCLUDE_DIR, headerDirectory);
				AddArgNt(&cmd, CLANG_DISABLE_WARNING, "unused-command-line-argument"); //Clang likes to warn about _lib_debug/_lib_release library folder being unused
				AddArgList(&cmd, &commonCompilerFlags);
				
				StrArray tags = EMPTY;
				AddStrArray(&tags, &commonTags);
				AddTag(&tags, T_CLANG);
				AddTag(&tags, T_OSX);
				AddTag(&tags, T_UNIX);
				AddTag(&tags, T_LANG_OBJECTIVEC);
				
				RunCliProgramAndExitOnFailureTags(StrLit(EXE_CLANG), tags, &cmd, FormatStr("Failed to build %.*s for OSX!", StrPrint(sourcePath)));
				AssertFileExist(oPath, true);
			}
		}
		
		FreeStrArray(&findContext.shaderPaths);
		FreeStrArray(&findContext.headerPaths);
		FreeStrArray(&findContext.sourcePaths);
		FreeStrArray(&findContext.objPaths);
		FreeStrArray(&findContext.oPaths);
	}
	
	// +--------------------------------------------------------------+
	// |                      Build pig_core.dll                      |
	// +--------------------------------------------------------------+
	if ((BUILD_APP_EXE || BUILD_APP_DLL) && !BUILD_PIG_CORE_DLL && !BUILD_INTO_SINGLE_UNIT && BUILDING_ON_WINDOWS && !DoesFileExist(StrLit(FILENAME_PIG_CORE_DLL))) { PrintLine("Building %s because it's missing", FILENAME_PIG_CORE_DLL); BUILD_PIG_CORE_DLL = true; BUILD_WINDOWS = true; }
	if ((BUILD_APP_EXE || BUILD_APP_DLL) && !BUILD_PIG_CORE_DLL && !BUILD_INTO_SINGLE_UNIT && !BUILDING_ON_WINDOWS && !DoesFileExist(StrLit(FILENAME_PIG_CORE_SO))) { PrintLine("Building %s because it's missing", FILENAME_PIG_CORE_SO); BUILD_PIG_CORE_DLL = true; BUILD_LINUX = true; }
	if (BUILD_PIG_CORE_DLL)
	{
		if (BUILD_WINDOWS)
		{
			InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized);
			PrintLine("\n[Building %s for Windows...]", FILENAME_PIG_CORE_DLL);
			
			CliArgs cmd = EMPTY;
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/core/dll/dll_main.c");
			AddArgNt(&cmd, CL_BINARY_FILE, FILENAME_PIG_CORE_DLL);
			AddArgNt(&cmd, CL_DEFINE, "PIG_CORE_DLL_INCLUDE_GFX_SYSTEM_GLOBAL=1");
			AddArgList(&cmd, &commonCompilerFlags);
			if (DUMP_ASSEMBLY) { AddArgNt(&cmd, CL_ASSEMB_LISTING_FILE, "pig_core.asm"); }
			AddArg(&cmd, CL_LINK);
			AddArg(&cmd, LINK_BUILD_DLL);
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_MSVC_CL);
			AddTag(&tags, T_WINDOWS);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_PIG_CORE);
			AddTag(&tags, T_LIBRARY);
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, StrLit("Failed to build " FILENAME_PIG_CORE_DLL "!"));
			AssertFileExist(StrLit(FILENAME_PIG_CORE_DLL), true);
			PrintLine("[Built %s for Windows!]", FILENAME_PIG_CORE_DLL);
		}
		if (BUILD_LINUX)
		{
			PrintLine("\n[Building %s for Linux...]", FILENAME_PIG_CORE_SO);
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/core/dll/dll_main.c");
			AddArgNt(&cmd, CLANG_OUTPUT_FILE, FILENAME_PIG_CORE_SO);
			AddArg(&cmd, CLANG_BUILD_SHARED_LIB);
			AddArg(&cmd, CLANG_fPIC);
			AddArgNt(&cmd, CLANG_DEFINE, "PIG_CORE_DLL_INCLUDE_GFX_SYSTEM_GLOBAL=1");
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_LINUX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_PIG_CORE);
			AddTag(&tags, T_LIBRARY);
			
			#if BUILDING_ON_LINUX
			Str clangExe = StrLit(EXE_CLANG);
			#else
			Str clangExe = StrLit(EXE_WSL_CLANG);
			mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
			chdir(FOLDERNAME_LINUX);
			cmd.rootDirPath = StrLit("../..");
			#endif
			
			RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, StrLit("Failed to build " FILENAME_PIG_CORE_SO "!"));
			AssertFileExist(StrLit(FILENAME_PIG_CORE_SO), true);
			PrintLine("[Built %s for Linux!]", FILENAME_PIG_CORE_SO);
			
			#if !BUILDING_ON_LINUX
			chdir("..");
			#endif
		}
		//TODO: Add OSX support
	}
	
	// +--------------------------------------------------------------+
	// |                  Build PROJECT_EXE_NAME.exe                  |
	// +--------------------------------------------------------------+
	if (RUN_APP && !BUILD_APP_EXE && BUILDING_ON_WINDOWS && !DoesFileExist(filenameAppExe)) { PrintLine("Building %.*s because it's missing", StrPrint(filenameAppExe)); BUILD_APP_EXE = true; BUILD_WINDOWS = true; }
	if (RUN_APP && !BUILD_APP_EXE && !BUILDING_ON_WINDOWS && !DoesFileExist(filenameApp)) { PrintLine("Building %.*s because it's missing", StrPrint(filenameApp)); BUILD_APP_EXE = true; BUILD_LINUX = true; }
	if (BUILD_APP_EXE)
	{
		if (BUILD_WINDOWS)
		{
			InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized);
			PrintLine("\n[Building %.*s for Windows...]", StrPrint(filenameAppExe));
			
			// Build app/win_resources.rc file into resources.res
			if (!DoesFileExist(StrLit(FILENAME_WIN_RESOURCES_RES)))
			{
				PrintLine("Generating %s...", FILENAME_WIN_RESOURCES_RES);
				CliArgs rcCmd = EMPTY;
				AddArg(&rcCmd, RC_NO_LOGO);
				AddArgNt(&rcCmd, RC_OUTPUT_FILE, FILENAME_WIN_RESOURCES_RES);
				AddArgNt(&rcCmd, CLI_QUOTED_ARG, "[ROOT]/app/win_resources.rc");
				RunCliProgramAndExitOnFailure(StrLit(EXE_MSVC_RC), &rcCmd, StrLit("Failed to generate resources.res for Windows embedded icon in .exe!"));
			}
			
			CliArgs cmd = EMPTY;
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/platform_main.c"); //NOTE: When BUILD_INTO_SINGLE_UNIT platform_main.c #includes app_main.c (and has PigCore implementations)
			AddArgStr(&cmd, CL_BINARY_FILE, filenameAppExe);
			AddArgList(&cmd, &commonCompilerFlags);
			if (DUMP_ASSEMBLY) { AddArgStr(&cmd, CL_ASSEMB_LISTING_FILE, filenameAppAsm); }
			AddArg(&cmd, CL_LINK);
			AddArgList(&cmd, &commonLinkerFlags);
			if (!BUILD_INTO_SINGLE_UNIT) { AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_PIG_CORE_LIB); }
			AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_WIN_RESOURCES_RES);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_MSVC_CL);
			AddTag(&tags, T_WINDOWS);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_PROGRAM);
			if (BUILD_INTO_SINGLE_UNIT)
			{
				AddTag(&tags, T_PIG_CORE);
				AddTag(&tags, T_SHADER_OBJS);
			}
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, FormatStr("Failed to build %.*s on Windows!", StrPrint(filenameAppExe)));
			AssertFileExist(filenameAppExe, true);
			PrintLine("[Built %.*s for Windows!]", StrPrint(filenameAppExe));
		}
		
		if (BUILD_LINUX)
		{
			PrintLine("\n[Building %.*s for Linux...]", StrPrint(filenameApp));
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/platform_main.c"); //NOTE: When BUILD_INTO_SINGLE_UNIT platform_main.c #includes app_main.c (and has PigCore implementations)
			AddArgStr(&cmd, CLANG_OUTPUT_FILE, filenameApp);
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgNt(&cmd, CLANG_SYSTEM_LIBRARY, "GL"); //TODO: We should update pig_build_pig_core_flags.h tags so this gets added based on our tags chosen below
			AddArgNt(&cmd, CLANG_RPATH_DIR, ".");
			if (!BUILD_INTO_SINGLE_UNIT) { AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_PIG_CORE_SO); }
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_LINUX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_PROGRAM);
			if (BUILD_INTO_SINGLE_UNIT)
			{
				AddTag(&tags, T_PIG_CORE);
				AddTag(&tags, T_SHADER_OBJS);
			}
			
			#if BUILDING_ON_LINUX
			Str clangExe = StrLit(EXE_CLANG);
			#else
			Str clangExe = StrLit(EXE_WSL_CLANG);
			mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
			chdir(FOLDERNAME_LINUX);
			cmd.rootDirPath = StrLit("../..");
			#endif
			
			RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, FormatStr("Failed to build %.*s on Linux!", StrPrint(filenameApp)));
			AssertFileExist(filenameApp, true);
			PrintLine("[Built %.*s for Linux!]", StrPrint(filenameApp));
			
			#if !BUILDING_ON_LINUX
			chdir("..");
			#endif
		}
		
		if (BUILD_OSX)
		{
			PrintLine("\n[Building %.*s for OSX...]", StrPrint(filenameApp));
			
			Str platformMainMPath = StrLit("platform_main.m");
			if (!DoesFileExist(platformMainMPath))
			{
				WriteLine_E("Creating platform_main.m");
				CreateAndWriteFile(platformMainMPath, StrLit("#include \"platform_main.c\"\n"), true);
			}
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgStr(&cmd, CLI_QUOTED_ARG, platformMainMPath); //NOTE: When BUILD_INTO_SINGLE_UNIT platform_main.c #includes app_main.c (and has PigCore implementations)
			AddArgStr(&cmd, CLANG_OUTPUT_FILE, filenameApp);
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgNt(&cmd, CLANG_SYSTEM_LIBRARY, "GL"); //TODO: We should update pig_build_pig_core_flags.h tags so this gets added based on our tags chosen below
			AddArgNt(&cmd, CLANG_RPATH_DIR, ".");
			if (!BUILD_INTO_SINGLE_UNIT) { AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_PIG_CORE_SO); }
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_OSX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_PROGRAM);
			if (BUILD_INTO_SINGLE_UNIT)
			{
				AddTag(&tags, T_PIG_CORE);
				AddTag(&tags, T_SHADER_OBJS);
			}
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_CLANG), tags, &cmd, FormatStr("Failed to build %.*s on OSX!", StrPrint(filenameApp)));
			AssertFileExist(filenameApp, true);
			PrintLine("[Built %.*s for OSX!]", StrPrint(filenameApp));
		}
	}
	
	// +--------------------------------------------------------------+
	// |                  Build PROJECT_DLL_NAME.dll                  |
	// +--------------------------------------------------------------+
	if (RUN_APP && !BUILD_APP_DLL && BUILDING_ON_WINDOWS && !DoesFileExist(filenameAppDll)) { PrintLine("Building %.*s because it's missing", StrPrint(filenameAppDll)); BUILD_APP_DLL = true; BUILD_WINDOWS = true; }
	if (RUN_APP && !BUILD_APP_DLL && !BUILDING_ON_WINDOWS && !DoesFileExist(filenameAppSo)) { PrintLine("Building %.*s because it's missing", StrPrint(filenameAppSo)); BUILD_APP_DLL = true; BUILD_LINUX = true; }
	if (BUILD_APP_DLL)
	{
		if (BUILD_WINDOWS)
		{
			InitializeMsvcIf(StrLit("../core"), &isMsvcInitialized);
			PrintLine("\n[Building %.*s for Windows...]", StrPrint(filenameAppDll));
			
			CliArgs cmd = EMPTY;
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/app_main.c");
			AddArgStr(&cmd, CL_BINARY_FILE, filenameAppDll);
			AddArgList(&cmd, &commonCompilerFlags);
			if (DUMP_ASSEMBLY) { AddArgStr(&cmd, CL_ASSEMB_LISTING_FILE, filenameAppDllAsm); }
			AddArg(&cmd, CL_LINK);
			AddArg(&cmd, LINK_BUILD_DLL);
			AddArgList(&cmd, &commonLinkerFlags);
			AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_PIG_CORE_LIB);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_MSVC_CL);
			AddTag(&tags, T_WINDOWS);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_LIBRARY);
			AddTag(&tags, T_SHADER_OBJS);
			
			RunCliProgramAndExitOnFailureTags(StrLit(EXE_MSVC_CL), tags, &cmd, FormatStr("Failed to build %.*s!", StrPrint(filenameAppDll)));
			AssertFileExist(filenameAppDll, true);
			PrintLine("[Built %.*s for Windows!]", StrPrint(filenameAppDll));
		}
		
		if (BUILD_LINUX)
		{
			PrintLine("\n[Building %.*s for Linux...]", StrPrint(filenameAppSo));
			
			CliArgs cmd = EMPTY;
			cmd.pathSepChar = '/';
			AddArgNt(&cmd, CLI_QUOTED_ARG, "[ROOT]/app/app_main.c");
			AddArgStr(&cmd, CLANG_OUTPUT_FILE, filenameAppSo);
			AddArg(&cmd, CLANG_BUILD_SHARED_LIB);
			AddArg(&cmd, CLANG_fPIC);
			AddArgNt(&cmd, CLI_QUOTED_ARG, FILENAME_PIG_CORE_SO);
			AddArgList(&cmd, &commonCompilerFlags);
			AddArgList(&cmd, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddTag(&tags, T_CLANG);
			AddTag(&tags, T_LINUX);
			AddTag(&tags, T_UNIX);
			AddTag(&tags, T_LANG_C);
			AddTag(&tags, T_LIBRARY);
			AddTag(&tags, T_SHADER_OBJS);
			
			#if BUILDING_ON_LINUX
			Str clangExe = StrLit(EXE_CLANG);
			#else
			Str clangExe = StrLit(EXE_WSL_CLANG);
			mkdir(FOLDERNAME_LINUX, FOLDER_PERMISSIONS);
			chdir(FOLDERNAME_LINUX);
			cmd.rootDirPath = StrLit("../..");
			#endif
			
			RunCliProgramAndExitOnFailureTags(clangExe, tags, &cmd, FormatStr("Failed to build %.*s!", StrPrint(filenameAppSo)));
			AssertFileExist(filenameAppSo, true);
			PrintLine("[Built %.*s for Linux!]", StrPrint(filenameAppSo));
			
			#if !BUILDING_ON_LINUX
			chdir("..");
			#endif
		}
		
		//TODO: Add OSX support
	}
	
	// +--------------------------------------------------------------+
	// |                   Copy to _data Directory                    |
	// +--------------------------------------------------------------+
	if (COPY_TO_DATA_DIRECTORY)
	{
		Str dataFolder = StrLit("../_data");
		PrintLine("Copying files to %.*s...", StrPrint(dataFolder));
		#if BUILDING_ON_WINDOWS
		if (BUILD_PIG_CORE_DLL) { CopyFileToFolder(StrLit(FILENAME_PIG_CORE_DLL), dataFolder, true); }
		if (BUILD_APP_EXE) { CopyFileToFolder(filenameAppExe, dataFolder, true); }
		if (BUILD_APP_DLL) { CopyFileToFolder(filenameAppDll, dataFolder, true); }
		#elif BUILDING_ON_LINUX
		if (BUILD_PIG_CORE_DLL) { CopyFileToFolder(StrLit(FILENAME_PIG_CORE_SO), dataFolder, true); }
		if (BUILD_APP_EXE) { CopyFileToFolder(filenameApp, dataFolder, true); }
		if (BUILD_APP_DLL) { CopyFileToFolder(filenameAppSo, dataFolder, true); }
		if (PROFILING_ENABLED) { CopyFileToFolder(StrLit(FILENAME_TRACY_SO), dataFolder, true); }
		#endif
	}
	
	// +--------------------------------------------------------------+
	// |                   Run PROJECT_EXE_NAME.exe                   |
	// +--------------------------------------------------------------+
	if (RUN_APP)
	{
		Str appBinaryName = BUILDING_ON_WINDOWS ? filenameAppExe : filenameApp;
		Str runAppStr = JoinStrings2(StrLit(EXEC_PROGRAM_IN_FOLDER_PREFIX), appBinaryName);
		PrintLine("\n[%.*s]", StrPrint(runAppStr));
		RunCliProgramAndExitOnFailure(runAppStr, nullptr, FormatStr("%.*s exited with error!", StrPrint(appBinaryName)));
	}
	
	PrintLine("\n[%s Finished Successfully]", BUILD_SCRIPT_EXE_NAME);
	return 0;
}
