@echo off
REM By default blocks like IF and FOR in batch are treated as a single command and variable values are expended when the block is entered
REM Enabling "delayed expansion" allows us to use !variable! syntax inside blocks to get the value of variables on that line (and at each iteration in the case of loops)
setlocal enabledelayedexpansion

if not exist _build mkdir _build
pushd _build
set root=..
set build_config_path=%root%\build_config.h
set initialized_msvc_compiler=0
set tool_compiler_flags=/std:clatest /O2 /FC /nologo /Zi /I"%root%" /I"%root%\core"

:: +--------------------------------------------------------------+
:: |                  Compile extract_define.exe                  |
:: +--------------------------------------------------------------+
set extract_define_tool_name=extract_define.exe

:: If we need to build any tools, then we'll need to init MSVC compiler early, otherwise we initialize it later ONLY if BUILD_WINDOWS is true in build_config.h
:: VsDevCmd.bat often takes 1-2 seconds to run (often longer than the actual compile), so skipping it when it's not needed saves us a lot of time
set need_to_build_extract_define=0
if not exist %extract_define_tool_name% (
	set need_to_build_extract_define=1
)

if "%need_to_build_extract_define%"=="1" (
	if "%initialized_msvc_compiler%"=="0" (
		CALL :init_msvc_compiler
		set initialized_msvc_compiler=1
	)
)

if "%need_to_build_extract_define%"=="1" (
	echo [Building %extract_define_tool_name%...]
	del %extract_define_tool_name% > NUL 2> NUL
	cl %tool_compiler_flags% %root%\core\tools\tools_extract_define_main.c /Fe%extract_define_tool_name%
	if !ERRORLEVEL! NEQ 0 (
		echo [FAILED to build %extract_define_tool_name%!]
		exit
	)
	if not exist %extract_define_tool_name% (
		echo [cl did NOT produce %extract_define_tool_name%!]
		exit
	)
	echo [Built %extract_define_tool_name%!]
)

:: +--------------------------------------------------------------+
:: |                    Scrape build_config.h                     |
:: +--------------------------------------------------------------+
set extract_define=%extract_define_tool_name% %build_config_path%
for /f "delims=" %%i in ('%extract_define% BUILD_PIG_BUILD') do set BUILD_PIG_BUILD=%%i

:: +--------------------------------------------------------------+
:: |                     Build pig_build.exe                      |
:: +--------------------------------------------------------------+
set pig_build_tool_name=pig_build.exe
set pig_build_pdb_name=pig_build.pdb
set need_to_build_pig_build=0
if not exist %pig_build_tool_name% (
	set need_to_build_pig_build=1
)
if "%BUILD_PIG_BUILD%"=="1" (
	set need_to_build_pig_build=1
)

if "%need_to_build_pig_build%"=="1" (
	if "%initialized_msvc_compiler%"=="0" (
		CALL :init_msvc_compiler
		set initialized_msvc_compiler=1
	)
)

if "%need_to_build_pig_build%"=="1" (
	echo [Building %pig_build_tool_name%...]
	cl %tool_compiler_flags% %root%\build_script.c /Fe%pig_build_tool_name% /Fd"%pig_build_pdb_name%" /link Shlwapi.lib
	if !ERRORLEVEL! NEQ 0 (
		echo [FAILED to build %pig_build_tool_name%!]
		exit
	)
	if not exist %pig_build_tool_name% (
		echo [cl did NOT produce %pig_build_tool_name%!]
		exit
	)
	echo [Built %pig_build_tool_name%!]
)

%pig_build_tool_name%

GOTO :eof

:: +--------------------------------------------------------------+
:: |                      Init MSVC Compiler                      |
:: +--------------------------------------------------------------+
:init_msvc_compiler

for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "vsdevcmd_start_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

REM set VSCMD_DEBUG=3
REM NOTE: Uncomment or change one of these lines to match your installation of Visual Studio compiler
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -no_logo
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 > NUL

for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "vsdevcmd_end_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
set /A vsdevcmd_elapsed_hundredths=vsdevcmd_end_time-vsdevcmd_start_time
set /A vsdevcmd_elapsed_seconds_part=vsdevcmd_elapsed_hundredths/100
set /A vsdevcmd_elapsed_hundredths_part=vsdevcmd_elapsed_hundredths%%100
if %vsdevcmd_elapsed_hundredths_part% lss 10 set vsdevcmd_elapsed_hundredths_part=0%vsdevcmd_elapsed_hundredths_part%
echo VsDevCmd.bat took %vsdevcmd_elapsed_seconds_part%.%vsdevcmd_elapsed_hundredths_part% seconds

EXIT /B

:: ------------------------------------------------------------------------------------------------------------------------------
if not exist _build mkdir _build
pushd _build
set root=..
set app=..\app
set core=..\core
set tools=%core%\third_party\_tools\win32
set scripts=%core%\_scripts

:: +--------------------------------------------------------------+
:: |                    Scrape build_config.h                     |
:: +--------------------------------------------------------------+
python --version > NUL 2> NUL
if errorlevel 1 (
	echo WARNING: Python isn't installed on this computer. Defines cannot be extracted from build_config.h! And build numbers won't be incremented
	exit
)

set extract_define=python %scripts%\extract_define.py ../build_config.h
for /f "delims=" %%i in ('%extract_define% DEBUG_BUILD') do set DEBUG_BUILD=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_INTO_SINGLE_UNIT') do set BUILD_INTO_SINGLE_UNIT=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_WINDOWS') do set BUILD_WINDOWS=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_LINUX') do set BUILD_LINUX=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_SHADERS') do set BUILD_SHADERS=%%i
for /f "delims=" %%i in ('%extract_define% BUNDLE_RESOURCES_ZIP') do set BUNDLE_RESOURCES_ZIP=%%i
for /f "delims=" %%i in ('%extract_define% BUNDLE_RESOURCES_ZIP_IF_NEEDED') do set BUNDLE_RESOURCES_ZIP_IF_NEEDED=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_PIG_CORE_LIB') do set BUILD_PIG_CORE_LIB=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_PIG_CORE_LIB_IF_NEEDED') do set BUILD_PIG_CORE_LIB_IF_NEEDED=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_APP_EXE') do set BUILD_APP_EXE=%%i
for /f "delims=" %%i in ('%extract_define% BUILD_APP_DLL') do set BUILD_APP_DLL=%%i
for /f "delims=" %%i in ('%extract_define% RUN_APP') do set RUN_APP=%%i
for /f "delims=" %%i in ('%extract_define% COPY_TO_DATA_DIRECTORY') do set COPY_TO_DATA_DIRECTORY=%%i
for /f "delims=" %%i in ('%extract_define% DUMP_PREPROCESSOR') do set DUMP_PREPROCESSOR=%%i
for /f "delims=" %%i in ('%extract_define% CONVERT_WASM_TO_WAT') do set CONVERT_WASM_TO_WAT=%%i
for /f "delims=" %%i in ('%extract_define% USE_EMSCRIPTEN') do set USE_EMSCRIPTEN=%%i
for /f "delims=" %%i in ('%extract_define% ENABLE_AUTO_PROFILE') do set ENABLE_AUTO_PROFILE=%%i
for /f "delims=" %%i in ('%extract_define% PROJECT_DLL_NAME') do set PROJECT_DLL_NAME=%%i
for /f "delims=" %%i in ('%extract_define% PROJECT_EXE_NAME') do set PROJECT_EXE_NAME=%%i

:: +--------------------------------------------------------------+
:: |                      Init MSVC Compiler                      |
:: +--------------------------------------------------------------+
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "vsdevcmd_start_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
if "%BUILD_WINDOWS%"=="1" (
	REM set VSCMD_DEBUG=3
	REM NOTE: Uncomment or change one of these lines to match your installation of Visual Studio compiler
	REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
	REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -no_logo
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 > NUL
)
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "vsdevcmd_end_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
set /A vsdevcmd_elapsed_hundredths=vsdevcmd_end_time-vsdevcmd_start_time
set /A vsdevcmd_elapsed_seconds_part=vsdevcmd_elapsed_hundredths/100
set /A vsdevcmd_elapsed_hundredths_part=vsdevcmd_elapsed_hundredths%%100
if %vsdevcmd_elapsed_hundredths_part% lss 10 set vsdevcmd_elapsed_hundredths_part=0%vsdevcmd_elapsed_hundredths_part%
if "%BUILD_WINDOWS%"=="1" ( echo VsDevCmd.bat took %vsdevcmd_elapsed_seconds_part%.%vsdevcmd_elapsed_hundredths_part% seconds )

:: +--------------------------------------------------------------+
:: |                       Compiler Options                       |
:: +--------------------------------------------------------------+
:: /std:clatest = Use latest C language spec features
:: /experimental:c11atomics = Enables _Atomic types
set c_cl_flags=/std:clatest /experimental:c11atomics
:: /wd4471 = a forward declaration of an unscoped enumeration must have an underlying type
:: /wd5054 = operator '|': deprecated between enumerations of different types
set cpp_cl_flags=/std:c++20 /wd4471 /wd5054
:: /FC = Full path for error messages
:: /nologo = Suppress the startup banner
:: /W4 = Warning level 4 [just below /Wall]
:: /WX = Treat warnings as errors
:: /Gd = Use __cdecl calling convention (this is the default)
set common_cl_flags=/FC /nologo /W4 /WX /Gd
:: -fdiagnostics-absolute-paths = Print absolute paths in diagnostics TODO: Figure out how to resolve these back to windows paths for Sublime error linking?
:: -std=gnu2x = Use C20+ language spec (NOTE: We originally had -std=c2x but that didn't define MAP_ANONYMOUS and mmap was failing)
:: NOTE: Clang Warning Options: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
:: -Wall = This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros.
:: -Wextra = This enables some extra warning flags that are not enabled by -Wall.
:: -Wshadow = Warn whenever a local variable or type declaration shadows another variable, parameter, type, class member (in C++), or instance variable (in Objective-C) or whenever a built-in function is shadowed
:: -Wimplicit-fallthrough = Must use [[fallthrough]] on a case label that falls through to the next case
set common_clang_flags=-fdiagnostics-absolute-paths -std=gnu2x -Wall -Wextra -Wshadow -Wimplicit-fallthrough
:: /wd4130 = Logical operation on address of string constant [W4] TODO: Should we re-enable this one? Don't know any scenarios where I want to do this
:: /wd4201 = Nonstandard extension used: nameless struct/union [W4] TODO: Should we re-enable this restriction for ANSI compatibility?
:: /wd4324 = Structure was padded due to __declspec[align[]] [W4]
:: /wd4458 = Declaration of 'identifier' hides class member [W4]
:: /wd4505 = Unreferenced local function has been removed [W4]
:: /wd4996 = Usage of deprecated function, class member, variable, or typedef [W3]
:: /wd4706 = assignment within conditional expression [W?]
:: /we5262 = Enable the [[fallthrough]] missing warning
set common_cl_flags=%common_cl_flags% /wd4130 /wd4201 /wd4324 /wd4458 /wd4505 /wd4996 /wd4706 /we5262
:: -Wno-switch = 8 enumeration values not handled in switch: 'ArenaType_None', 'ArenaType_Funcs', 'ArenaType_Generic'...
:: -Wno-unused-function = unused function 'MeowExpandSeed'
set common_clang_flags=%common_clang_flags% -Wno-switch -Wno-unused-function
:: /I = Adds an include directory to search in when resolving #includes
set common_cl_flags=%common_cl_flags% /I"." /I"%root%" /I"%app%" /I"%core%"
:: -I = Add directory to the end of the list of include search paths
:: -lm = Include the math library (required for stuff like sinf, atan, etc.)
:: -ldl = Needed for dlopen and similar functions
:: -mssse3 = For MeowHash to work we need sse3 support
:: -maes = For MeowHash to work we need aes support
set linux_clang_flags=-lm -ldl -L "." -I ".." -I "../%root%" -I "../%app%" -I "../%core%" -mssse3 -maes
if "%DEBUG_BUILD%"=="1" (
	REM /MDd = ?
	REM /Od = Optimization level: Debug
	REM /Zi = Generate complete debugging information
	REM /wd4065 = Switch statement contains 'default' but no 'case' labels
	REM /wd4100 = Unreferenced formal parameter [W4]
	REM /wd4101 = Unreferenced local variable [W3]
	REM /wd4127 = Conditional expression is constant [W4]
	REM /wd4189 = Local variable is initialized but not referenced [W4]
	REM /wd4702 = Unreachable code [W4]
	set common_cl_flags=%common_cl_flags% /MDd /Od /Zi /wd4065 /wd4100 /wd4101 /wd4127 /wd4189 /wd4702
	REM -Wno-unused-parameter = warning: unused parameter 'numBytes'
	set common_clang_flags=%common_clang_flags% -Wno-unused-parameter -Wno-unused-variable
) else (
	REM /MD = ?
	REM /Ot = Favors fast code over small code
	REM /Oy = Omit frame pointer [x86 only]
	REM /O2 = Optimization level 2: Creates fast code
	REM /Zi = Generate complete debugging information [optional]
	set common_cl_flags=%common_cl_flags% /MD /Ot /Oy /O2
	set common_clang_flags=%common_clang_flags%
)

:: -incremental:no = Suppresses warning about doing a full link when it can't find the previous .exe result. We don't need this when doing unity builds
:: /LIBPATH = Add a library search path
:: User32.lib = Needed for os_font.h for CreateFontA, etc.
:: Gdi32.lib = Needed for os_font.h for GetForegroundWindow and GetDC
:: Ole32.lib = Needed for CoInitializeEx, CoCreateInstance, etc.
:: Shell32.lib = Needed for SHGetSpecialFolderPathA
:: Shlwapi.lib = Needed for PathFileExistsA
:: Comctl32.lib = Needed for native Windows tooltips TODO: Add this back when we get Windows tooltips works
set common_ld_flags=-incremental:no /NOLOGO User32.lib Gdi32.lib Ole32.lib Shell32.lib Shlwapi.lib
set pig_core_ld_flags=
set platform_ld_flags=

if "%DEBUG_BUILD%"=="1" (
	set common_ld_flags=%common_ld_flags% /LIBPATH:"%root%\third_party\_lib_debug" /LIBPATH:"%core%\third_party\_lib_debug"
) else (
	set common_ld_flags=%common_ld_flags% /LIBPATH:"%root%\third_party\_lib_release" /LIBPATH:"%core%\third_party\_lib_release"
)

set pig_core_defines=/DPIG_CORE_DLL_INCLUDE_GFX_SYSTEM_GLOBAL=1

if "%DUMP_PREPROCESSOR%"=="1" (
	REM /P = Output the result of the preprocessor to {file_name}.i (disables the actual compilation)
	REM /C = Preserve comments through the preprocessor
	set common_cl_flags=/P /C %common_cl_flags%
	REM -E = Only run the preprocessor
	set common_clang_flags=%common_clang_flags% -E
)

for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "build_start_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

:: +--------------------------------------------------------------+
:: |                        Pack Resources                        |
:: +--------------------------------------------------------------+
set resources_zip_path=resources.zip
set resources_h_path=%app%\resources_zip.h
set resources_c_path=resources_zip.c
if "%BUNDLE_RESOURCES_ZIP_IF_NEEDED%"=="1" (
	if not exist %resources_c_path% (
		set BUNDLE_RESOURCES_ZIP=1
	)
)
if "%BUNDLE_RESOURCES_ZIP%"=="1" (
	python %scripts%\pack_resources.py %root%\_data\resources %resources_zip_path% %resources_h_path% %resources_c_path%
)

:: +--------------------------------------------------------------+
:: |                        Build Shaders                         |
:: +--------------------------------------------------------------+
set shader_list_file=shader_list_source.txt
if "%BUILD_SHADERS%"=="1" (
	if not exist sokol-shdc.exe (
		echo "[Copying sokol-shdc.exe from %tools%...]"
		COPY %tools%\sokol-shdc.exe sokol-shdc.exe
	)
	echo.
	echo [Compiling Shaders...]
	python %scripts%\find_and_compile_shaders.py "%root%" --exclude="third_party" --exclude="core" --exclude=".git" --exclude="_build" --list_file=%shader_list_file%
)

:: Read the list file into %shader_list% variable
for /f "delims=" %%x in (%shader_list_file%) do set shader_list=%%x

set shader_object_files=
set shader_linux_object_files=
REM Separate list by commas, for each item compile to .obj file, add output path to %shader_object_files%
for %%y in ("%shader_list:,=" "%") do (
	set object_name=%%~ny%.obj
	set linux_object_name=%%~ny%.o
	if "%BUILD_SHADERS%"=="1" (
		set shader_file_path=%%~y%
		set shader_file_path_fw_slash=!shader_file_path:\=/!
		set shader_file_dir=%%y:~0,-1%
		if "%BUILD_WINDOWS%"=="1" (
			cl /c %common_cl_flags% %c_cl_flags% /I"!shader_file_dir!" /Fo"!object_name!" !shader_file_path!
		)
		if "%BUILD_LINUX%"=="1" (
			if not exist linux mkdir linux
			pushd linux
			wsl clang-18 -c %common_clang_flags% %linux_clang_flags% -I "../!shader_file_dir!" -o "!linux_object_name!" ../!shader_file_path_fw_slash!
			popd
		)
	)
	set shader_object_files=!shader_object_files! !object_name!
	set shader_linux_object_files=!shader_linux_object_files! !linux_object_name!
)
if "%BUILD_SHADERS%"=="1" ( echo [Shaders Compiled!] )
rem echo shader_object_files %shader_object_files%
rem echo shader_linux_object_files %shader_linux_object_files%

:: +--------------------------------------------------------------+
:: |                      Build pig_core.dll                      |
:: +--------------------------------------------------------------+
set pig_core_source_path=%core%/dll/dll_main.c
set pig_core_dll_path=pig_core.dll
set pig_core_lib_path=pig_core.lib
set pig_core_so_path=libpig_core.so
set pig_core_cl_args=%common_cl_flags% %c_cl_flags% %pig_core_defines% /Fe%pig_core_dll_path% %pig_core_source_path% /link %common_ld_flags% %pig_core_ld_flags% /DLL
:: -fPIC = "Position Independent Code" (Required for globals to work properly?)
:: -shared = ?
set pig_core_clang_args=%common_clang_flags% %linux_clang_flags% -fPIC -shared -o %pig_core_so_path% ../%pig_core_source_path%

if "%BUILD_PIG_CORE_LIB_IF_NEEDED%"=="1" (
	if "%BUILD_WINDOWS%"=="1" (
		if not exist %pig_core_dll_path% (
			set BUILD_PIG_CORE_LIB=1
		)
	)
	if "%BUILD_LINUX%"=="1" (
		if not exist linux\%pig_core_so_path% (
			set BUILD_PIG_CORE_LIB=1
		)
	)
)

if "%BUILD_INTO_SINGLE_UNIT%"=="1" (
	if "%BUILD_WINDOWS%"=="1" (
		del "%pig_core_dll_path%" > NUL 2> NUL
		del "%pig_core_lib_path%" > NUL 2> NUL
	)
	if "%BUILD_LINUX%"=="1" (
		del "linux\%pig_core_so_path%" > NUL 2> NUL
	)
) else (
	if "%BUILD_PIG_CORE_LIB%"=="1" (
		if "%BUILD_WINDOWS%"=="1" (
			del "%pig_core_dll_path%" > NUL 2> NUL
			del "%pig_core_lib_path%" > NUL 2> NUL
			
			echo.
			echo [Building %pig_core_dll_path% for Windows...]
			cl %pig_core_cl_args%
			echo [Built %pig_core_dll_path% for Windows!]
			
			if "%COPY_TO_DATA_DIRECTORY%"=="1" (
				COPY %pig_core_dll_path% %root%\_data\%pig_core_dll_path% > NUL
			)
		)
		if "%BUILD_LINUX%"=="1" (
			echo.
			echo [Building %pig_core_so_path% for Linux...]
			if not exist linux mkdir linux
			pushd linux
			
			del "%pig_core_so_path%" > NUL 2> NUL
			wsl clang-18 %pig_core_clang_args%
			
			popd
			echo [Built %pig_core_so_path% for Linux!]
		)
	)
)

:: +--------------------------------------------------------------+
:: |                 Build %PROJECT_EXE_NAME%.exe                 |
:: +--------------------------------------------------------------+
set platform_source_path=%app%/platform_main.c
set resources_rc_path=%app%\win_resources.rc
set resources_res_path=win_resources.res
set platform_exe_path=%PROJECT_EXE_NAME%.exe
set platform_bin_path=%PROJECT_EXE_NAME%
set platform_cl_args=%common_cl_flags% %c_cl_flags% /Fe%platform_exe_path% %platform_source_path% /link %common_ld_flags% %platform_ld_flags% %resources_res_path%
set platform_clang_args=%common_clang_flags% %linux_clang_flags% -o %platform_bin_path% ../%platform_source_path%
if "%BUILD_INTO_SINGLE_UNIT%"=="1" (
	set platform_cl_args=%platform_cl_args% %pig_core_ld_flags% %shader_object_files%
	set platform_clang_args=%platform_clang_args% %shader_linux_object_files%
) else (
	REM -rpath = Add to RPATH so that libpig_core.so can be found in this folder (it doesn't need to be copied to any system folder)
	set platform_cl_args=%platform_cl_args% %pig_core_lib_path%
	set platform_clang_args=-lpig_core -rpath "." %platform_clang_args% 
)

if "%BUILD_APP_EXE%"=="1" (
	if "%BUILD_WINDOWS%"=="1" (
		del %platform_exe_path% > NUL 2> NUL
		
		echo.
		echo [Building %platform_exe_path% for Windows...]
		rc /nologo /Fo%resources_res_path% %resources_rc_path%
		cl %platform_cl_args%
		echo [Built %platform_exe_path% for Windows!]
		
		if "%COPY_TO_DATA_DIRECTORY%"=="1" (
			COPY %platform_exe_path% %root%\_data\%platform_exe_path% > NUL
		)
	)
	if "%BUILD_LINUX%"=="1" (
		echo.
		echo [Building %platform_bin_path% for Linux...]
		if not exist linux mkdir linux
		pushd linux
		
		del %platform_bin_path% > NUL 2> NUL
		wsl clang-18 %platform_clang_args%
		
		popd
		echo [Built %platform_bin_path% for Linux!]
	)
)

:: +--------------------------------------------------------------+
:: |                 Build %PROJECT_DLL_NAME%.dll                 |
:: +--------------------------------------------------------------+
set app_source_path=%app%/app_main.c
set app_dll_path=%PROJECT_DLL_NAME%.dll
set app_so_path=%PROJECT_DLL_NAME%.so
set app_dll_cl_args=%common_cl_flags% %c_cl_flags% /Fe%app_dll_path% %app_source_path% /link %common_ld_flags% %pig_core_lib_path% %shader_object_files% /DLL
set app_dll_clang_args=%common_clang_flags% %linux_clang_flags% -shared -lpig_core  -o %app_so_path% ../%app_source_path% %shader_linux_object_files%

if "%BUILD_INTO_SINGLE_UNIT%"=="1" (
	if "%BUILD_WINDOWS%"=="1" (
		del "%app_dll_path%" > NUL 2> NUL
	)
	if "%BUILD_LINUX%"=="1" (
		del "linux\%app_so_path%" > NUL 2> NUL
	)
) else (
	if "%BUILD_APP_DLL%"=="1" (
		if "%BUILD_WINDOWS%"=="1" (
			del %app_dll_path% > NUL 2> NUL
			
			echo.
			echo [Building %app_dll_path% for Windows...]
			cl %app_dll_cl_args%
			echo [Built %app_dll_path% for Windows!]
			
			if "%COPY_TO_DATA_DIRECTORY%"=="1" (
				COPY %app_dll_path% %root%\_data\%app_dll_path% > NUL
			)
		)
		if "%BUILD_LINUX%"=="1" (
			echo.
			echo [Building %app_so_path% for Linux...]
			if not exist linux mkdir linux
			pushd linux
			
			del %app_so_path% > NUL 2> NUL
			wsl clang-18 %app_dll_clang_args%
			
			popd
			echo [Built %app_so_path% for Linux!]
		)
	)
)

:: +--------------------------------------------------------------+
:: |                  Measure Build Elapsed Time                  |
:: +--------------------------------------------------------------+
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
	set /A "build_end_time=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
set /A build_elapsed_hundredths=build_end_time-build_start_time
set /A build_elapsed_seconds_part=build_elapsed_hundredths/100
set /A build_elapsed_hundredths_part=build_elapsed_hundredths%%100
if %build_elapsed_hundredths_part% lss 10 set build_elapsed_hundredths_part=0%build_elapsed_hundredths_part%
echo.
echo Build took %build_elapsed_seconds_part%.%build_elapsed_hundredths_part% seconds

:: +--------------------------------------------------------------+
:: |                          Run Things                          |
:: +--------------------------------------------------------------+

if "%RUN_APP%"=="1" (
	echo.
	echo [%platform_exe_path%]
	%platform_exe_path%
)

echo.
popd