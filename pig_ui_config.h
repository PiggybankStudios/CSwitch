/*
File:   pig_ui_config.h
Author: Taylor Robbins
Date:   06\15\2026
*/

#ifndef _PIG_UI_CONFIG_H
#define _PIG_UI_CONFIG_H

#include "base/base_defines_check.h"
#include "base/base_macros.h"

typedef plex UiRendererParameters_Cswitch UiRendererParameters_Cswitch;
plex UiRendererParameters_Cswitch
{
	TextContraction textContraction;
};
#define UiRendererParameters UiRendererParameters_Cswitch

#endif //  _PIG_UI_CONFIG_H
