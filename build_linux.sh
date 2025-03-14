#!/bin/bash

mkdir -p _build
pushd _build > /dev/null
root=".."
app="../app"
core="../core"
scripts="$core/_scripts"
tools="$core/third_party/_tools/linux"

python3 --version > /dev/null 2> /dev/null
if [ $? -ne 0 ]
then
	echo "WARNING: Python isn't installed on this computer. Defines cannot be extracted from build_config.h! And build numbers won't be incremented"
	exit
fi

extract_defines="python3 $scripts/extract_define.py $root/build_config.h"
# TODO: We need to somehow figure out if any of these failed
DEBUG_BUILD=$($extract_defines DEBUG_BUILD)
BUILD_LINUX=$($extract_defines BUILD_LINUX)
BUILD_SHADERS=$($extract_defines BUILD_SHADERS)
BUILD_PIG_CORE_LIB=$($extract_defines BUILD_PIG_CORE_LIB)
BUILD_APP_EXE=$($extract_defines BUILD_APP_EXE)
BUILD_APP_DLL=$($extract_defines BUILD_APP_DLL)
COPY_TO_DATA_DIRECTORY=$($extract_defines COPY_TO_DATA_DIRECTORY)
DUMP_PREPROCESSOR=$($extract_defines DUMP_PREPROCESSOR)
PROJECT_DLL_NAME=$($extract_defines PROJECT_DLL_NAME)
PROJECT_EXE_NAME=$($extract_defines PROJECT_EXE_NAME)

c_clang_flags="-std=gnu2x"
cpp_clang_flags="TODO"
# -fdiagnostics-absolute-paths = Print absolute paths in diagnostics TODO: Figure out how to resolve these back to windows paths for Sublime error linking?
# -std=gnu2x = Use C20+ language spec (NOTE: We originally had -std=c2x but that didn't define MAP_ANONYMOUS and mmap was failing)
# NOTE: Clang Warning Options: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# -Wall = This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros.
# -Wextra = This enables some extra warning flags that are not enabled by -Wall.
# -Wshadow = Warn whenever a local variable or type declaration shadows another variable, parameter, type, class member (in C++), or instance variable (in Objective-C) or whenever a built-in function is shadowed
# -Wimplicit-fallthrough = Must use [[fallthrough]] on a case label that falls through to the next case
common_clang_flags="-fdiagnostics-absolute-paths -Wall -Wextra -Wshadow -Wimplicit-fallthrough"
# -Wno-switch = 8 enumeration values not handled in switch: 'ArenaType_None', 'ArenaType_Funcs', 'ArenaType_Generic'...
# -Wno-unused-function = unused function 'MeowExpandSeed'
common_clang_flags="$common_clang_flags -Wno-switch -Wno-unused-function"
# -I = Add directory to the end of the list of include search paths
# -mssse3 = For MeowHash to work we need sse3 support
# -maes = For MeowHash to work we need aes support
linux_clang_flags="-I$root -I$core -mssse3 -maes"
# -fPIC = ?
# -shared = ?
pig_core_clang_flags="-DPIG_CORE_DLL_INCLUDE_GFX_SYSTEM_GLOBAL=1 -fPIC -shared"
platform_clang_flags="-Wl,-rpath='/home/robbitay/MyStuff/gamedev/projects/CSwitch/_data/'"
app_clang_flags="-fPIC -shared -Wl,-rpath='/home/robbitay/MyStuff/gamedev/projects/CSwitch/_data/'"
# -lm = Include the math library (required for stuff like sinf, atan, etc.)
# -ldl = Needed for dlopen and similar functions
common_linker_flags="-lm -ldl"
if [[ $DEBUG_BUILD -eq 1 ]] then
	# -Wno-unused-parameter = warning: unused parameter 'numBytes'
	# -Wno-unused-variable = ?
	common_clang_flags="$common_clang_flags -g -Wno-unused-parameter -Wno-unused-variable"
	common_linker_flags="$common_linker_flags -L$root/third_party/_lib_debug"
else
	common_clang_flags="$common_clang_flags"
	common_linker_flags="$common_linker_flags -L$root/third_party/_lib_release"
fi
if [[ $DUMP_PREPROCESSOR -eq 1 ]] then
	# -E = Only run the preprocessor
	common_clang_flags="$common_clang_flags -E"
fi

# -lfontconfig = FontConfig library is needed for OsReadPlatformFont implementation in os_font.h
# -lGL = OpenGL
# -lX11 = X11 Windowing Library
# -lXi = X11 Extensions?
# -lXcursor = X11 Cursor
common_libraries="-lfontconfig -lGL -lX11 -lXi -lXcursor"

# +--------------------------------------------------------------+
# |                        Pack Resources                        |
# +--------------------------------------------------------------+
# TODO: Implement packing of resources

# +--------------------------------------------------------------+
# |                        Build Shaders                         |
# +--------------------------------------------------------------+
shader_list_file="shader_list_source.txt"
shader_clang_args="$common_clang_flags $c_clang_flags $linux_clang_flags"
if [[ $BUILD_SHADERS -eq 1 ]] then
	if [[ ! -f sokol-shdc ]] then
		cp $tools/sokol-shdc sokol-shdc
	fi
	python3 $scripts/find_and_compile_shaders.py "$root" --exclude="third_party" --exclude="core" --exclude=".git" --exclude="_build" --list_file="$shader_list_file"
fi
shader_source_files=`cat $shader_list_file`

shader_o_paths=""
for shader_path in ${shader_source_files//,/ }
do
	shader_pathname="$(basename "$shader_path")"
	shader_o_path="${shader_pathname%.*}.o"
	if [[ $BUILD_SHADERS -eq 1 ]] then
		echo "[Compiling $shader_path...]"
		clang-18 -c $shader_clang_args $shader_path -o "$shader_o_path"
	fi
	if [[ -z "${shader_o_paths}" ]] then
		shader_o_paths="$shader_o_path"
	else
		shader_o_paths="$shader_o_paths $shader_o_path"
	fi
done

# +--------------------------------------------------------------+
# |                      Build pig_core.so                       |
# +--------------------------------------------------------------+
pig_core_so_path=libpig_core.so
pig_core_lib_flag=-lpig_core
pig_core_dll_source_path=$core/dll/dll_main.c
pig_core_so_clang_args="$common_clang_flags $c_clang_flags $linux_clang_flags $pig_core_clang_flags -o $pig_core_so_path $pig_core_dll_source_path $common_linker_flags $common_libraries"
if [[ $BUILD_PIG_CORE_LIB -eq 1 ]] then
	if [[ $BUILD_LINUX -eq 1 ]] then
		echo "[Building pig_core.so for Linux...]"
		clang-18 $pig_core_so_clang_args
		echo "[Built pig_core.so for Linux!]"
		if [[ $COPY_TO_DATA_DIRECTORY -eq 1 ]] then
			cp $pig_core_so_path $root/_data/$pig_core_so_path
		fi
	fi
fi

# +--------------------------------------------------------------+
# |                   Build $PROJECT_EXE_NAME                    |
# +--------------------------------------------------------------+
platform_bin_path=$PROJECT_EXE_NAME
platform_source_path=$app/platform_main.c
platform_clang_args="$common_clang_flags $c_clang_flags $linux_clang_flags $platform_clang_flags -o $platform_bin_path $platform_source_path $common_linker_flags $common_libraries $pig_core_so_path $shader_o_paths"
if [[ $BUILD_APP_EXE -eq 1 ]] then
	if [[ $BUILD_LINUX -eq 1 ]] then
		echo "[Building $platform_bin_path for Linux...]"
		clang-18 $platform_clang_args
		echo "[Built $platform_bin_path for Linux!]"
		if [[ $COPY_TO_DATA_DIRECTORY -eq 1 ]] then
			cp $platform_bin_path $root/_data/$platform_bin_path
		fi
	fi
fi

# +--------------------------------------------------------------+
# |                  Build $PROJECT_DLL_NAME.so                  |
# +--------------------------------------------------------------+
app_so_path=$PROJECT_DLL_NAME.so
app_source_path=$app/app_main.c
app_clang_args="$common_clang_flags $c_clang_flags $linux_clang_flags $app_clang_flags -o $app_so_path $app_source_path $common_linker_flags $common_libraries $pig_core_so_path $shader_o_paths"
if [[ $BUILD_APP_DLL -eq 1 ]] then
	if [[ $BUILD_LINUX -eq 1 ]] then
		echo "[Building $app_so_path for Linux...]"
		clang-18 $app_clang_args
		echo "[Built $app_so_path for Linux!]"
		if [[ $COPY_TO_DATA_DIRECTORY -eq 1 ]] then
			cp $app_so_path $root/_data/$app_so_path
		fi
	fi
fi

popd > /dev/null
