/*
File:   app_resources.h
Author: Taylor Robbins
Date:   03\01\2025
*/

#ifndef _APP_RESOURCES_H
#define _APP_RESOURCES_H

#if USE_EMBEDDED_RESOURCES_ZIP
#include "resources_zip.h"
#endif

typedef struct AppResources AppResources;
struct AppResources
{
	bool isLoadingFromDisk;
	#if USE_EMBEDDED_RESOURCES_ZIP
	ZipArchive zipFile;
	#endif
};

#endif //  _APP_RESOURCES_H
