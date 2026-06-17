/*
File:   app_ui.c
Author: Taylor Robbins
Date:   06\12\2026
Description: 
	** Holds all the UI code for CSwitch
	** There are currently 2 implementations, one using
	** Pig UI (ui_system.h from PigCore) and the legacy
	** one using Clay (clay.h in third_party)
*/

//TODO: Topbar doesn't completely disappear
//TODO: If we have a FIT element wrapping an EXPAND element, what should happen? Should the outer container have an infinite preferred size, or zero preferred size?
//TODO: Make scrolling framerate independent
//TODO: Add a debug menu for Pig UI
//TODO: Can we have grid-style layout options built-in to the UI system somehow? For small buttons for example?
//TODO: Add tooltips!
//TODO: Border thickness should be rounded just like sizes are rounded to produce full pixel sizes
//TODO: Old version of CSwitch crashes when settings.txt contains UiScale entry?
//TODO: Make a generic renderer implementation into PigCore?
//TODO: Ctrl+Scroll should now scroll the options
//TODO: Scrolling while hovering scrollbar should work
//TODO: Moving selection with keyboard should scroll options viewport

void DoCSwitchAppUI(v2 screenSize)
{
	u16 fullscreenBorderThickness = (appIn->isWindowTopmost ? 1 : 0);
	
	Assert(NUM_SCRATCH_ARENAS_PER_THREAD == 3);
	Arena* scratch1 = GetScratch(nullptr);
	Arena* scratch2 = GetScratch1(scratch1, nullptr);
	uiArena = GetScratch2(scratch1, scratch2, nullptr);
	FlagSet(uiArena->flags, ArenaFlag_DontPop);
	uxx uiArenaMark = ArenaGetMark(uiArena);
	
	// +--------------------------------------------------------------+
	// |                        Pig UI Version                        |
	// +--------------------------------------------------------------+
	#if BUILD_WITH_PIG_UI
	StartUiFrame(&app->ui,
		screenSize,
		GetThemeColor(OptionListBack),
		app->settings.uiScale,
		appIn->programTime,
		app->settings.smoothScrollingDisabled ? -1.0f : (r32)OPTIONS_SMOOTH_SCROLLING_DIVISOR,
		&appIn->keyboard,
		&appIn->mouse,
		&appIn->touch
	);
	
	UIELEM({ .id = UiIdLit("FullscreenContainer"),
		.direction = UiLayoutDir_TopDown,
		.alignment = UI_ALIGN_TOP_LEFT(),
		.sizing = UI_EXPAND2(),
		.padding = { .inner = FillV4r(fullscreenBorderThickness), },
		.borderThickness = FillV4r(fullscreenBorderThickness),
		.borderColor = GetThemeColor(TopmostBorder),
	})
	{
		// +==============================+
		// |            Topbar            |
		// +==============================+
		if (!app->minimalModeEnabled || app->isFileMenuOpen || app->isViewMenuOpen)
		{
			UiTopbar(UiIdLit("Topbar"), !app->minimalModeEnabled)
			{
				// +==============================+
				// |          File Menu           |
				// +==============================+
				UiTopbarMenuBtn(UiIdLit("FileBtn"), StrLit("File"), &app->isFileMenuOpen, &app->keepOpenRecentSubmenuOpenUntilMouseOver, app->isOpenRecentSubmenuOpen)
				{
					if (UiDropdownBtn(UiIdLit("OpenFileBtn"), true, AppIcon_OpenFile, StrLit("Open" UNICODE_ELLIPSIS_STR), AppCommand_OpenFile, StrLit("Open a file")))
					{
						RunAppCommand(AppCommand_OpenFile);
						app->isFileMenuOpen = false;
					}
					
					//TODO: Open Recent submenu
					
					if (UiDropdownBtn(UiIdLit("ResetFileBtn"), (app->currentTab != nullptr && app->currentTab->isFileChangedFromOriginal), AppIcon_ResetFile, StrLit("Reset File"), AppCommand_ResetFile, StrLit("Reset file to how it was when first opened")))
					{
						RunAppCommand(AppCommand_ResetFile);
						app->isFileMenuOpen = false;
					}
					
					if (UiDropdownBtn(UiIdLit("ReloadingEnabledBtn"), (app->tabs.length > 0), AppIcon_None, ScratchPrintStr("%s File Reloading", app->settings.dontAutoReloadFile ? "Enable" : "Disable"), AppCommand_ToggleFileReloading, StrLit("When an open file is changed externally, should CSwitch automatically read the new state of the file and display it. There is a small performance cost for watching the file for changes")))
					{
						RunAppCommand(AppCommand_ToggleFileReloading);
					}
					
					if (UiDropdownBtn(UiIdLit("CloseFileBtn"), (app->currentTab != nullptr), AppIcon_CloseFile, StrLit("Close File"), AppCommand_CloseTab, StrLit("Close the current file tab")))
					{
						RunAppCommand(AppCommand_CloseTab);
						app->isFileMenuOpen = false;
					}
				}
				
				// +==============================+
				// |          View Menu           |
				// +==============================+
				UiTopbarMenuBtn(UiIdLit("ViewBtn"), StrLit("View"), &app->isViewMenuOpen, &app->keepViewMenuOpenUntilMouseOver, false)
				{
					ThemeMode otherThemeMode = ((DEBUG_BUILD && IsKeyDownRaw(Key_Shift))
						? ThemeMode_Debug
						: ((app->currentThemeMode == ThemeMode_Dark) ? ThemeMode_Light : ThemeMode_Dark)
					);
					if (UiDropdownBtn(UiIdLit("LightModeBtn"),
						true, //enabled
						AppIcon_LightDark,
						ScratchPrintStr("%s Mode", GetThemeModeStr(otherThemeMode)),
						AppCommand_ToggleLightMode,
						StrLit("Toggle between dark and light mode")))
					{
						RunAppCommand(AppCommand_ToggleLightMode);
					}
					
					if (UiDropdownBtn(UiIdLit("SmallButtonsBtn"),
						true, //enabled
						AppIcon_SmallBtn,
						ScratchPrintStr("%s Buttons", app->settings.smallButtons ? "Large" : "Small"),
						AppCommand_ToggleSmallButtons,
						StrLit("Toggle between small buttons with abbreviations laid out in a grid and large buttons with full names in a vertical list")))
					{
						RunAppCommand(AppCommand_ToggleSmallButtons);
					}
					
					if (UiDropdownBtn(UiIdLit("TopmostBtn"),
						true, //enabled
						appIn->isWindowTopmost ? AppIcon_TopmostEnabled : AppIcon_TopmostDisabled,
						ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "Disable" : "Enable"),
						AppCommand_ToggleTopmost,
						StrLit("Toggle forcing this window to display on top of other windows even when it's not focused (Windows only)")))
					{
						RunAppCommand(AppCommand_ToggleTopmost);
					}
					
					if (UiDropdownBtn(UiIdLit("ClipNamesBtn"),
						true, //enabled
						app->settings.clipNamesLeft ? AppIcon_ClipLeft : AppIcon_ClipRight,
						ScratchPrintStr("Clip Names on %s", app->settings.clipNamesLeft ? "Left" : "Right"),
						AppCommand_ToggleClipSide,
						StrLit("Changes which side of the full name we should omit on a button when there is not enough horizontal space")))
					{
						RunAppCommand(AppCommand_ToggleClipSide);
					}
					
					if (UiDropdownBtn(UiIdLit("SmoothScrollingBtn"),
						true, //enabled
						AppIcon_SmoothScroll,
						ScratchPrintStr("%s Smooth Scrolling", app->settings.smoothScrollingDisabled ? "Enable" : "Disable"),
						AppCommand_ToggleSmoothScrolling,
						StrLit("Toggles whether the scrollable list should animate over time after being moved with mouse scroll wheel")))
					{
						RunAppCommand(AppCommand_ToggleSmoothScrolling);
					}
					
					if (UiDropdownBtn(UiIdLit("TooltipsBtn"),
						true, //enabled
						AppIcon_Tooltip,
						ScratchPrintStr("%s Option Tooltips", app->settings.optionTooltipsDisabled ? "Enable" : "Disable"),
						AppCommand_ToggleOptionTooltips,
						StrLit("Toggles whether tooltips with full name should be displayed when hovering over a button in the list")))
					{
						RunAppCommand(AppCommand_ToggleOptionTooltips);
					}
					
					if (UiDropdownBtn(UiIdLit("HideTopbarBtn"),
						true, //enabled
						AppIcon_Topbar,
						ScratchPrintStr("%s Topbar", app->minimalModeEnabled ? "Show" : "Hide"),
						AppCommand_ToggleTopbar,
						StrLit("Toggles visibility of this topbar, usually only useful if we need to maximize use of vertical space")))
					{
						RunAppCommand(AppCommand_ToggleTopbar);
					}
					
					Str8 currentThemeStr = !IsEmptyStr(app->settings.userThemePath)
						? PrintInArenaStr(uiArena, "current: \"%.*s\"", StrPrint(app->settings.userThemePath))
						: StrLit("current: -");
					Str8 openThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Open a custom user theme file"), '\n', currentThemeStr, false);
					if (UiDropdownBtn(UiIdLit("OpenThemeBtn"),
						true, //enabled
						AppIcon_StarFile,
						StrLit("Open Custom Theme" UNICODE_ELLIPSIS_STR),
						AppCommand_OpenCustomTheme,
						openThemeTooltipStr))
					{
						RunAppCommand(AppCommand_OpenCustomTheme);
						app->isFileMenuOpen = false;
					}
					
					Str8 clearThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Remove the custom user theme"), '\n', currentThemeStr, false);
					if (UiDropdownBtn(UiIdLit("ClearThemeBtn"),
						!IsEmptyStr(app->settings.userThemePath), //enabled
						AppIcon_CloseFile,
						StrLit("Clear Custom Theme"),
						AppCommand_ClearCustomTheme,
						clearThemeTooltipStr))
					{
						RunAppCommand(AppCommand_ClearCustomTheme);
					}
					
					#if DEBUG_BUILD
					
					// if (UiDropdownBtn(UiIdLit("ClayDebugBtn"),
					// 	true, //enabled
					// 	AppIcon_Debug,
					// 	ScratchPrintStr("%s Clay UI Debug", Clay_IsDebugModeEnabled() ? "Hide" : "Show"),
					// 	AppCommand_ToggleClayDebug,
					// 	StrLit("Toggles the debug sidebar for Clay")))
					// {
					// 	RunAppCommand(AppCommand_ToggleClayDebug);
					// }
					
					if (UiDropdownBtn(UiIdLit("SleepBtn"),
						true, //enabled
						AppIcon_None,
						ScratchPrintStr("%s Sleeping", app->sleepingDisabled ? "Enable" : "Disable"),
						AppCommand_ToggleSleeping,
						StrLit("Toggles whether the rendering loop is allowed to \"sleep\" when nothing is changing")))
					{
						RunAppCommand(AppCommand_ToggleSleeping);
					}
					
					if (UiDropdownBtn(UiIdLit("FrameIndicatorBtn"),
						true, //enabled
						AppIcon_None,
						ScratchPrintStr("%s Frame Indicator", app->enableFrameUpdateIndicator ? "Disable" : "Enable"),
						AppCommand_ToggleFrameIndicator,
						StrLit("Toggles a indicator that changes every frame, helpful when debugging \"sleep\" behavior or trying to visualize the framerate")))
					{
						RunAppCommand(AppCommand_ToggleFrameIndicator);
					}
					
					#endif //DEBUG_BUILD
				}
				
				// +==============================+
				// |       Frame Indicator        |
				// +==============================+
				if (app->enableFrameUpdateIndicator)
				{
					//NOTE: This little visual makes it easier to tell when we are rendering new frames and when we are asleep by having a little bar move every frame
					UIELEM({ .id=UiIdLit("FrameUpdateIndicatorContainer"),
						.direction = UiLayoutDir_TopDown,
						.sizing = { .width=UI_FIXED(4), .height=UI_PERCENT(1.0f) },
						.color = Black,
					})
					{
						for (uxx pIndex = 0; pIndex < 10; pIndex++)
						{
							UIELEM_LEAF({
								.sizing = { .width=UI_EXPAND(), .height=UI_PERCENT(0.1f) },
								.color = ((appIn->frameIndex % 10) == pIndex) ? MonokaiWhite : Black,
							});
						}
					}
				}
				
				// +==============================+
				// |      File Path Display       |
				// +==============================+
				if (app->currentTab != nullptr)
				{
					UIELEM({ .id = UiIdLit("FilePathDisplay"),
						.direction = UiLayoutDir_LeftToRight,
						.sizing = { .width=UI_FIT(), .height=UI_FIT() },
						.alignment = UI_ALIGN_RIGHT_CENTER(),
					})
					{
						//TODO: How do we specify elipses logic to shrink path text?
						UIELEM_LEAF({
							.text = app->currentTab->filePath,
							.padding = { .outer={ .Right=4 } },
							.font = &app->uiFont,
							.fontSize = app->uiFontSize,
							.fontStyle = UI_FONT_STYLE,
							.textColor = GetThemeColor(TopbarPathText),
							.sizing = UI_TEXT_CLIP(0),
							.renderer = { .textContraction = TextContraction_EllipseFilePath },
						});
					}
				}
			}
		}
		
		// +==============================+
		// |             Tabs             |
		// +==============================+
		if (app->tabs.length > 1)
		{
			UIELEM({ .id = UiIdLit("TabGutter"),
				.direction = UiLayoutDir_LeftToRight,
				.alignment = UI_ALIGN_TOP_LEFT(),
				.sizing = { .width=UI_EXPAND(), .height=UI_FIT() },
				.padding = { .inner = { .Top=4 } },
				.color = GetThemeColor(TopbarBack),
			})
			{
				bool wasPrevHovered = false;
				VarArrayLoop(&app->tabs, tIndex)
				{
					VarArrayLoopGet(FileTab, tab, &app->tabs, tIndex);
					bool isCurrentTab = (app->currentTabIndex == tIndex);
					UiId tabId = UiIdStrIndex(JoinStringsInArena(uiArena, StrLit("Tab_"), tab->filePath, false), tIndex);
					bool isHovered = IsUiElementHovered(tabId);
					ThemeState tabThemeState = isCurrentTab ? ThemeState_Open : (isHovered ? ThemeState_Hovered : ThemeState_Default);
					Color32 backgroundColor = GetThemeColorEx(FileTabBack,   tabThemeState);
					Color32 borderColor     = GetThemeColorEx(FileTabBorder, tabThemeState);
					Color32 textColor       = GetThemeColorEx(FileTabText,   tabThemeState);
					
					// Dividers in-between not-selected and not-hovered tabs
					if (tIndex > 0)
					{
						bool shouldShowDivider = (!isCurrentTab && app->currentTabIndex != tIndex-1 && !isHovered && !wasPrevHovered);
						UIELEM_LEAF({ .id = UiIdLitIndex("TabDivider", tIndex),
							.sizing = { .width=UI_FIXED(1), .height=UI_PERCENT(1.0f) },
							.color = (shouldShowDivider ? GetThemeColor(FileTabDivider) : Transparent),
						});
					}
					
					UIELEM({ .id = tabId,
						.direction = UiLayoutDir_LeftToRight,
						.alignment = UI_ALIGN_CENTER(),
						.sizing = { .width=UI_PERCENT(1.0f / (r32)app->tabs.length), .height=UI_FIT() },
						.padding = { .inner=FillV4r(4) },
						.cornerRadius = { .TopLeft=4, .TopRight=4 },
						.color = backgroundColor,
						.borderColor = borderColor,
						.borderThickness = { .Left=2, .Top=2, .Right=2 },//TODO: Add support to Pig UI Renderer for missing sides when both corners don't have a radius!
					})
					{
						Str8 displayPath = GetUniqueTabFilePath(tab->filePath);
						UIELEM_LEAF({
							.text = displayPath,
							.font = &app->uiFont,
							.fontSize = app->uiFontSize,
							.fontStyle = UI_FONT_STYLE,
							.textColor = textColor,
							.sizing = UI_TEXT_FULL(),
							.renderer = { .textContraction = TextContraction_EllipseRight },
						});
						
						if (isHovered && MouseLeftClicked())
						{
							AppChangeTab(tIndex);
						}
						if (isHovered && MouseMiddleClicked())
						{
							AppCloseFileTab(tIndex);
							tIndex--;
						}
					}
					
					wasPrevHovered = isHovered;
				}
			}
		}
		
		// +==============================+
		// |         Options List         |
		// +==============================+
		SetScrollbarColors(&app->optionsListScrollbarState);
		app->optionsListScrollbarState.autohide = true;
		ContainerWithVerticalScrollbar(UiIdLit("OptionsList"), &app->optionsListScrollbarState, { })
		{
			UIELEM({ .id = UiIdLit("OptionsList"),
				.direction = UiLayoutDir_TopDown,
				.sizing = UI_EXPAND2(),
				.alignment = UI_ALIGN_TOP_LEFT(),
				.scrolling = UI_SCROLL_VERTICAL(),
				.padding = { .inner = FillV4r(4), .child=OPTION_UI_GAP },
			})
			{
				if (app->currentTab != nullptr)
				{
					// +==============================+
					// |        Large Options         |
					// +==============================+
					if (app->settings.smallButtons == false)
					{
						VarArrayLoop(&app->currentTab->fileOptions, oIndex)
						{
							VarArrayLoopGet(FileOption, option, &app->currentTab->fileOptions, oIndex);
							bool isOptionSelected = (app->usingKeyboardToSelect && app->currentTab->selectedOptionIndex >= 0 && (uxx)app->currentTab->selectedOptionIndex == oIndex);
							
							if (option->type == FileOptionType_Bool)
							{
								if (UiOptionBtn(UiIdStrIndex(option->name, oIndex), option->name, option->valueStr, option->valueBool, isOptionSelected))
								{
									ToggleOption(app->currentTab, option);
								}
							}
							else if (option->type == FileOptionType_CommentDefine)
							{
								if (UiOptionBtn(UiIdStrIndex(option->name, oIndex), ScratchPrintStr("%s%.*s", option->isUncommented ? "" : "// ", StrPrint(option->name)), Str8_Empty, option->isUncommented, isOptionSelected))
								{
									ToggleOption(app->currentTab, option);
								}
							}
							else
							{
								if (UiOptionBtn(UiIdStrIndex(option->name, oIndex), option->name, StrLit("-"), false, isOptionSelected))
								{
									ToggleOption(app->currentTab, option);
								}
							}
							if (option->numEmptyLinesAfter > 0)
							{
								UIELEM_LEAF({
									.sizing = { .height=UI_FIXED((r32)option->numEmptyLinesAfter * LINE_BREAK_EXTRA_UI_GAP) },
								});
							}
						}
					}
					// +==============================+
					// |        Small Options         |
					// +==============================+
					else
					{
						UiElement* optionsListElem = GetUiElementByIdInPrevFrame(UiIdLit("OptionsList"), true);
						r32 optionsAreaWidth = (optionsListElem != nullptr)
							? (optionsListElem->layoutRec.Width - optionsListElem->config.padding.inner.Left - optionsListElem->config.padding.inner.Right)
							: screenSize.Width;
						u16 buttonMargin = (u16)RoundR32i(SMALL_BTN_MARGIN * app->settings.uiScale);
						r32 buttonWidth = app->currentTab->longestAbbreviationWidth + (r32)RoundR32(SMALL_BTN_PADDING_X * app->settings.uiScale)*2;
						r32 unscaledButtonWidth = buttonWidth / app->settings.uiScale;
						i32 numColumns = FloorR32i((optionsAreaWidth - (r32)buttonMargin) / (buttonWidth + (r32)buttonMargin));
						if (numColumns <= 0) { numColumns = 1; }
						// PrintLine_D("buttonWidth: %g/%g", buttonWidth, unscaledButtonWidth);
						// PrintLine_D("optionsAreaWidth: %g (%g)", optionsAreaWidth, optionsListElem->layoutRec.Width);
						// PrintLine_D("longestAbbreviationWidth: %g", app->currentTab->longestAbbreviationWidth);
						// PrintLine_D("numColumns: %d", numColumns);
						
						bool containerStarted = false;
						VarArrayLoop(&app->currentTab->fileOptions, oIndex)
						{
							VarArrayLoopGet(FileOption, option, &app->currentTab->fileOptions, oIndex);
							bool isOptionSelected = (app->usingKeyboardToSelect && app->currentTab->selectedOptionIndex >= 0 && (uxx)app->currentTab->selectedOptionIndex == oIndex);
							
							if ((oIndex % numColumns) == 0)
							{
								if (containerStarted) { CloseUiElement(); }
								OpenUiElement((UiElemConfig){
									.direction = UiLayoutDir_LeftToRight,
									.sizing = { .width=UI_EXPAND(), .height=UI_FIT() },
									.alignment = UI_ALIGN_LEFT_CENTER(),
									.padding = { .child=SMALL_BTN_MARGIN },
								});
								containerStarted = true;
							}
							
							if (option->type == FileOptionType_Bool)
							{
								if (UiSmallOptionBtn(UiIdStrIndex(option->name, oIndex), option->abbreviation, option->valueBool, isOptionSelected, unscaledButtonWidth))
								{
									ToggleOption(app->currentTab, option);
								}
							}
							else if (option->type == FileOptionType_CommentDefine)
							{
								if (UiSmallOptionBtn(UiIdStrIndex(option->name, oIndex), option->abbreviation, option->isUncommented, isOptionSelected, unscaledButtonWidth))
								{
									ToggleOption(app->currentTab, option);
								}
							}
							else
							{
								if (UiSmallOptionBtn(UiIdStrIndex(option->name, oIndex), option->abbreviation, false, isOptionSelected, unscaledButtonWidth))
								{
									ToggleOption(app->currentTab, option);
								}
							}
						}
						if (containerStarted) { CloseUiElement(); }
					}
				}
			}
		}
	}
	
	RenderPigUi(GetUiRenderList());
	EndUiFrame();
	
	#endif //BUILD_WITH_PIG_UI
	
	// +--------------------------------------------------------------+
	// |                       Clay UI Version                        |
	// +--------------------------------------------------------------+
	#if BUILD_WITH_CLAY
	UiWidgetContext uiContext = MakeUiWidgetContext(
		uiArena,
		&app->clay,
		&appIn->keyboard,
		&appInputHandling->keyboard,
		&appIn->mouse,
		&appInputHandling->mouse,
		app->settings.uiScale,
		nullptr, //TODO: Fill focusedElementPntr
		MouseCursorShape_Default,
		OsWindowHandleEmpty,
		appIn->programTime,
		&app->tooltips
	);
	
	v2 clayMouseScrollInput = appIn->mouse.scrollDelta;
	#if TARGET_IS_LINUX
	clayMouseScrollInput = ScaleV2(clayMouseScrollInput, LINUX_SCROLL_WHEEL_SCALING);
	#endif
	if (IsKeyDownRaw(Key_Control)) { clayMouseScrollInput = V2_Zero; }
	if (appInputHandling->mouse.scrollXHandled) { clayMouseScrollInput.X = 0; }
	if (appInputHandling->mouse.scrollYHandled) { clayMouseScrollInput.Y = 0; }
	
	app->wasClayScrollingPrevFrame = UpdateClayScrolling(&app->clay.clay, appIn->elapsedMs, false, clayMouseScrollInput, false);
	BeginClayUIRender(&app->clay.clay, screenSize, false, appIn->mouse.position, IsMouseDownRaw(MouseBtn_Left));
	
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
				// +==============================+
				// |          File Menu           |
				// +==============================+
				bool showMenuHotkeys = (IsKeyDown(Key_Alt) && appIn->isFocused);
				if (ClayTopBtn("File", showMenuHotkeys, &app->isFileMenuOpen, &app->keepFileMenuOpenUntilMouseOver, app->isOpenRecentSubmenuOpen))
				{
					if (ClayBtnAppIconStr(StrLit("OpenFileBtn"),
						StrLit("Open" UNICODE_ELLIPSIS_STR),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_OpenFile, uiArena, 0),
						StrLit("Open a file"),
						true, //isEnabled
						AppIcon_OpenFile))
					{
						RunAppCommand(AppCommand_OpenFile);
						app->isFileMenuOpen = false;
					} Clay__CloseElement();
					
					if (app->recentFiles.length > 0)
					{
						if (ClayTopSubmenu("OpenRecentSubmenu",
							"Open Recent " UNICODE_RIGHT_ARROW_STR,
							app->isFileMenuOpen, &app->isOpenRecentSubmenuOpen, &app->keepOpenRecentSubmenuOpenUntilMouseOver,
							&app->appIconsSheet.texture,
							GetSheetCellRec(&app->appIconsSheet, app->appIconSheetCell[AppIcon_OpenRecent])))
						{
							for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
							{
								RecentFile* recentFile = VarArrayGet(RecentFile, &app->recentFiles, rIndex-1);
								Str8 displayPath = GetUniqueRecentFilePath(recentFile->path);
								bool isOpenFile = (AppFindTabForPath(recentFile->path) != nullptr);
								Str8 tooltipStr = PrintInArenaStr(uiArena, "%.*s%s", StrPrint(recentFile->path), recentFile->fileExists ? "" : " (MISSING)");
								if (ClayBtnStrEx(recentFile->path,
									AllocStr8(uiArena, displayPath),
									StrLit(""), //hotkey
									tooltipStr,
									!isOpenFile && recentFile->fileExists,
									nullptr, Rec_Zero))
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
							if (ClayBtnAppIconStr(StrLit("ClearRecentBtn"),
								StrLit("Clear Recent Files"),
								GetBindingStrForAppCommand(&app->bindings, AppCommand_ClearRecentFiles, uiArena, 0),
								clearRecentFilesTooltipStr,
								app->recentFiles.length > 0, //isEnabled
								AppIcon_Trash))
							{
								RunAppCommand(AppCommand_ClearRecentFiles);
								app->isOpenRecentSubmenuOpen = false;
								app->isFileMenuOpen = false;
							} Clay__CloseElement();
							
							Clay__CloseElement();
							Clay__CloseElement();
						} Clay__CloseElement();
					}
					else
					{
						if (ClayBtnAppIcon("OpenRecentBtn",
							"Open Recent " UNICODE_RIGHT_ARROW_STR,
							"", //hotkey
							"", //tootltip
							false, //isEnabled
							AppIcon_OpenRecent))
						{
						} Clay__CloseElement();
					}
					
					if (ClayBtnAppIconStr(StrLit("ResetFileBtn"),
						StrLit("Reset File"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ResetFile, uiArena, 0),
						StrLit("Reset file to how it was when first opened"),
						(app->currentTab != nullptr && app->currentTab->isFileChangedFromOriginal),
						AppIcon_ResetFile))
					{
						RunAppCommand(AppCommand_ResetFile);
						app->isFileMenuOpen = false;
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("ReloadingEnabledBtn"),
						ScratchPrintStr("%s File Reloading", app->settings.dontAutoReloadFile ? "Enable" : "Disable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleFileReloading, uiArena, 0),
						StrLit("When an open file is changed externally, should CSwitch automatically read the new state of the file and display it. There is a small performance cost for watching the file for changes"),
						(app->tabs.length > 0),
						AppIcon_None)) //TODO: Add an icon for this option
					{
						RunAppCommand(AppCommand_ToggleFileReloading);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("CloseFileBtn"),
						StrLit("Close File"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_CloseTab, uiArena, 0),
						StrLit("Close the current file tab"),
						(app->currentTab != nullptr),
						AppIcon_CloseFile))
					{
						RunAppCommand(AppCommand_CloseTab);
						app->isFileMenuOpen = false;
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("ReloadBindingsBtn"),
						StrLit("Reload Bindings"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ReloadBindings, uiArena, 0),
						StrLit("Reload key bindings from " USER_BINDINGS_FILENAME " in the config folder. Bindings can also be reloaded by close and reopening C-Switch"),
						true, //isEnabled
						AppIcon_None))
					{
						RunAppCommand(AppCommand_ReloadBindings);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("CloseWindowBtn"),
						StrLit("Close Window"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_CloseWindow, uiArena, 0),
						Str8_Empty, //tooltip
						true, //isEnabled
						AppIcon_CloseWindow))
					{
						RunAppCommand(AppCommand_CloseWindow);
						app->isFileMenuOpen = false;
					} Clay__CloseElement();
					
					Clay__CloseElement();
					Clay__CloseElement();
				} Clay__CloseElement();
				
				// +==============================+
				// |          View Menu           |
				// +==============================+
				if (ClayTopBtn("View", showMenuHotkeys, &app->isViewMenuOpen, &app->keepViewMenuOpenUntilMouseOver, false))
				{
					ThemeMode otherThemeMode = ((DEBUG_BUILD && IsKeyDownRaw(Key_Shift))
						? ThemeMode_Debug
						: ((app->currentThemeMode == ThemeMode_Dark)
							? ThemeMode_Light
							: ThemeMode_Dark
						)
					);
					if (ClayBtnAppIconStr(StrLit("LightModeBtn"),
						ScratchPrintStr("%s Mode", GetThemeModeStr(otherThemeMode)),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleLightMode, uiArena, 0),
						StrLit("Toggle between dark and light mode"),
						true, //isEnabled
						AppIcon_LightDark))
					{
						RunAppCommand(AppCommand_ToggleLightMode);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("SmallButtonsBtn"),
						ScratchPrintStr("%s Buttons", app->settings.smallButtons ? "Large" : "Small"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleSmallButtons, uiArena, 0),
						StrLit("Toggle between small buttons with abbreviations laid out in a grid and large buttons with full names in a vertical list"),
						true, //isEnabled
						AppIcon_SmallBtn))
					{
						RunAppCommand(AppCommand_ToggleSmallButtons);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("TopmostBtn"),
						ScratchPrintStr("%s Topmost", appIn->isWindowTopmost ? "Disable" : "Enable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleTopmost, uiArena, 0),
						StrLit("Toggle forcing this window to display on top of other windows even when it's not focused (Windows only)"),
						true, //isEnabled
						appIn->isWindowTopmost ? AppIcon_TopmostEnabled : AppIcon_TopmostDisabled))
					{
						RunAppCommand(AppCommand_ToggleTopmost);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("ClipNamesBtn"),
						ScratchPrintStr("Clip Names on %s", app->settings.clipNamesLeft ? "Left" : "Right"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleClipSide, uiArena, 0),
						StrLit("Changes which side of the full name we should omit on a button when there is not enough horizontal space"),
						true, //isEnabled
						app->settings.clipNamesLeft ? AppIcon_ClipLeft : AppIcon_ClipRight))
					{
						RunAppCommand(AppCommand_ToggleClipSide);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("SmoothScrollingBtn"),
						ScratchPrintStr("%s Smooth Scrolling", app->settings.smoothScrollingDisabled ? "Enable" : "Disable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleSmoothScrolling, uiArena, 0),
						StrLit("Toggles whether the scrollable list should animate over time after being moved with mouse scroll wheel"),
						true, //isEnabled
						AppIcon_SmoothScroll))
					{
						RunAppCommand(AppCommand_ToggleSmoothScrolling);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("TooltipsBtn"),
						ScratchPrintStr("%s Option Tooltips", app->settings.optionTooltipsDisabled ? "Enable" : "Disable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleOptionTooltips, uiArena, 0),
						StrLit("Toggles whether tooltips with full name should be displayed when hovering over a button in the list"),
						true, //isEnabled
						AppIcon_Tooltip))
					{
						RunAppCommand(AppCommand_ToggleOptionTooltips);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("HideTopbarBtn"),
						ScratchPrintStr("%s Topbar", app->minimalModeEnabled ? "Show" : "Hide"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleTopbar, uiArena, 0),
						StrLit("Toggles visibility of this topbar, usually only useful if we need to maximize use of vertical space"),
						true, //isEnabled
						AppIcon_Topbar))
					{
						RunAppCommand(AppCommand_ToggleTopbar);
					} Clay__CloseElement();
					
					Str8 currentThemeStr = !IsEmptyStr(app->settings.userThemePath)
						? PrintInArenaStr(uiArena, "current: \"%.*s\"", StrPrint(app->settings.userThemePath))
						: StrLit("current: -");
					Str8 openThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Open a custom user theme file"), '\n', currentThemeStr, false);
					if (ClayBtnAppIconStr(StrLit("OpenThemeBtn"),
						StrLit("Open Custom Theme" UNICODE_ELLIPSIS_STR),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_OpenCustomTheme, uiArena, 0),
						openThemeTooltipStr,
						true, //isEnabled
						AppIcon_StarFile))
					{
						RunAppCommand(AppCommand_OpenCustomTheme);
						app->isFileMenuOpen = false;
					} Clay__CloseElement();
					
					Str8 clearThemeTooltipStr = JoinStringsInArenaWithChar(uiArena, StrLit("Remove the custom user theme"), '\n', currentThemeStr, false);
					if (ClayBtnAppIconStr(StrLit("ClearThemeBtn"),
						StrLit("Clear Custom Theme"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ClearCustomTheme, uiArena, 0),
						clearThemeTooltipStr,
						!IsEmptyStr(app->settings.userThemePath),
						AppIcon_CloseFile))
					{
						RunAppCommand(AppCommand_ClearCustomTheme);
					} Clay__CloseElement();
					
					#if DEBUG_BUILD
					if (ClayBtnAppIconStr(StrLit("ClayDebugBtn"),
						ScratchPrintStr("%s Clay UI Debug", Clay_IsDebugModeEnabled() ? "Hide" : "Show"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleClayDebug, uiArena, 0),
						StrLit("Toggles the debug sidebar for Clay"),
						true, //isEnabled
						AppIcon_Debug))
					{
						RunAppCommand(AppCommand_ToggleClayDebug);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("SleepBtn"),
						ScratchPrintStr("%s Sleeping", app->sleepingDisabled ? "Enable" : "Disable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleSleeping, uiArena, 0),
						StrLit("Toggles whether the rendering loop is allowed to \"sleep\" when nothing is changing"),
						true, //isEnabled
						AppIcon_None))
					{
						RunAppCommand(AppCommand_ToggleSleeping);
					} Clay__CloseElement();
					
					if (ClayBtnAppIconStr(StrLit("FrameIndicatorBtn"),
						ScratchPrintStr("%s Frame Indicator", app->enableFrameUpdateIndicator ? "Disable" : "Enable"),
						GetBindingStrForAppCommand(&app->bindings, AppCommand_ToggleFrameIndicator, uiArena, 0),
						StrLit("Toggles a indicator that changes every frame, helpful when debugging \"sleep\" behavior or trying to visualize the framerate"),
						true, //isEnabled
						AppIcon_None))
					{
						RunAppCommand(AppCommand_ToggleFrameIndicator);
					} Clay__CloseElement();
					#endif //DEBUG_BUILD
					
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
						
						if (isHovered && MouseLeftClicked())
						{
							AppChangeTab(tIndex);
						}
						if (isHovered && MouseMiddleClicked())
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
	
	Clay_RenderCommandArray clayRenderCommands = EndClayUIRender(&app->clay.clay);
	RenderClayCommandArray(&app->clay, &gfx, &clayRenderCommands);
	#endif //BUILD_WITH_CLAY
	
	FlagUnset(uiArena->flags, ArenaFlag_DontPop);
	ArenaResetToMark(uiArena, uiArenaMark);
	uiArena = nullptr;
}
