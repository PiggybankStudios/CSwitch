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

#include "pig_build.h"
#include "pig_build_optional.h"

int main(int argc, char* argv[])
{
	PigBuildDebugMode = false;
	RecompileIfNeeded(StrArray_Empty);
	PrintLine("[%s...]", BUILD_SCRIPT_EXE_NAME);
	
	StrArray cliArgs = EMPTY;
	//NOTE: We skip the first argument which is just the executable relative path
	for (int aIndex = 1; aIndex < argc; aIndex++)
	{
		AddStrNt(&cliArgs, argv[aIndex]);
	}
	
	Str buildConfigContents = ReadEntireFile(StrLit("../build_config.h"));
	
	// NOTE: See pig_build/src/optional/pig_build_pig_core_gui_app.h
	int result = BuildPigCoreGuiApplication(
		&cliArgs,
		buildConfigContents,
		StrLit("[ROOT]/app")
	);
	return result;
}
