/*
File:   app_commands.c
Author: Taylor Robbins
Date:   02\01\2026
Description: 
	** Commands are bits of logic that can be bound to app-wide hotkeys.
	** Many of these have a corresponding button in the topbar menus.
*/

void SelectAnOption()
{
	//TODO: Could we somehow choose the option thats near the middle of the screen?
	app->currentTab->selectedOptionIndex = 0;
}

// +==================================+
// | AppResetCurrentFilePopupCallback |
// +==================================+
// void AppResetCurrentFilePopupCallback(PopupDialogResult result, PopupDialog* dialog, PopupDialogButton* selectedButton, void* contextPntr)
POPUP_DIALOG_CALLBACK_DEF(AppResetCurrentFilePopupCallback)
{
	UNUSED(dialog); UNUSED(selectedButton); UNUSED(contextPntr);
	if (result == PopupDialogResult_Yes && app->currentTab != nullptr)
	{
		bool writeSuccess = OsWriteTextFile(app->currentTab->filePath, app->currentTab->originalFileContents);
		if (!writeSuccess) { NotifyPrint_E("Failed to write to file at \"%.*s\"!", StrPrint(app->currentTab->filePath)); }
		else
		{
			AppReloadFileTab(app->currentTabIndex);
		}
	}
}

// +==================================+
// | AppClearRecentFilesPopupCallback |
// +==================================+
// void AppClearRecentFilesPopupCallback(PopupDialogResult result, PopupDialog* dialog, PopupDialogButton* selectedButton, void* contextPntr)
POPUP_DIALOG_CALLBACK_DEF(AppClearRecentFilesPopupCallback)
{
	UNUSED(dialog); UNUSED(selectedButton); UNUSED(contextPntr);
	if (result == PopupDialogResult_Yes)
	{
		AppClearRecentFiles();
		AppSaveRecentFilesList();
	}
}

void RunAppCommand(AppCommand command) //pre-declared in app_commands.h
{
	NotNull(app);
	NotNull(appIn);
	
	switch (command)
	{
		// +==============================+
		// |  AppCommand_ReloadBindings   |
		// +==============================+
		case AppCommand_ReloadBindings:
		{
			if (AppTryLoadBindings(false))
			{
				Notify_I("Reloaded bindings");
			}
		} break;
		
		// +==============================+
		// | AppCommand_ClosePopupOrMenu  |
		// +==============================+
		case AppCommand_ClosePopupOrMenu:
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
			else if (DismissNotification(&app->notificationQueue, appIn->programTime, false))
			{
			}
			else if (app->minimalModeEnabled)
			{
				app->minimalModeEnabled = false;
			}
			else if (app->usingKeyboardToSelect)
			{
				app->usingKeyboardToSelect = false;
			}
		} break;
		
		
		// +==============================+
		// |     AppCommand_OpenFile      |
		// +==============================+
		case AppCommand_OpenFile:
		{
			if (app->openFileDialog.arena == nullptr)
			{
				#if (TARGET_IS_WINDOWS || TARGET_IS_LINUX)
				app->openFileDialogIsForTheme = false;
				OsDoOpenFileDialogAsync(stdHeap, true, &app->openFileDialog);
				Assert(app->openFileDialog.arena != nullptr || app->openFileDialog.error == Result_Canceled);
				#else //!(TARGET_IS_WINDOWS || TARGET_IS_LINUX)
				Notify_W("Open File dialog is not implemented on OSX currently! Please use drag-and-drop or pass the file path as a command-line argument!");
				#endif
			}
		} break;
		
		// +==============================+
		// | AppCommand_ReopenRecentFile  |
		// +==============================+
		case AppCommand_ReopenRecentFile:
		{
			for (uxx rIndex = app->recentFiles.length; rIndex > 0; rIndex--)
			{
				VarArrayLoopGet(RecentFile, recentFile, &app->recentFiles, rIndex-1);
				if (recentFile->fileExists && AppFindTabForPath(recentFile->path) == nullptr)
				{
					if (AppOpenFileTab(recentFile->path) != nullptr) { break; }
				}
			}
		} break;
		
		// +==============================+
		// |     AppCommand_CloseTab      |
		// +==============================+
		case AppCommand_CloseTab: //TODO: app->currentTab != nullptr
		{
			if (app->currentTab != nullptr)
			{
				AppCloseFileTab(app->currentTabIndex);
			}
		} break;
		
		// +==============================+
		// |    AppCommand_CloseWindow    |
		// +==============================+
		case AppCommand_CloseWindow:
		{
			platform->RequestQuit();
		} break;
		
		// +==============================+
		// |     AppCommand_ResetFile     |
		// +==============================+
		case AppCommand_ResetFile:
		{
			if (!app->popup.isOpen)
			{
				OpenPopupDialog(stdHeap, &app->popup,
					StrLit("Do you want to reset the file to the state it was in when it was opened?"),
					AppResetCurrentFilePopupCallback, nullptr
				);
				AddPopupButton(&app->popup, 1, StrLit("Cancel"), PopupDialogResult_No, GetThemeColor(ConfirmDialogNeutralBtnBorder));
				AddPopupButton(&app->popup, 2, StrLit("Reset"), PopupDialogResult_Yes, GetThemeColor(ConfirmDialogNegativeBtnBorder));
			}
		} break;
		
		// +==============================+
		// |  AppCommand_ToggleLightMode  |
		// +==============================+
		case AppCommand_ToggleLightMode:
		{
			//TODO: Holding Shift to get ThemeMode_Debug doesn't work with new binding key combo system
			ThemeMode otherThemeMode = ((DEBUG_BUILD && IsKeyDownRaw(Key_Shift))
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
		} break;
		
		// +===============================+
		// | AppCommand_ToggleSmallButtons |
		// +===============================+
		case AppCommand_ToggleSmallButtons:
		{
			app->settings.smallButtons = !app->settings.smallButtons;
			SaveAppSettings();
		} break;
		
		// +==============================+
		// |   AppCommand_ToggleTopmost   |
		// +==============================+
		case AppCommand_ToggleTopmost:
		{
			platform->SetWindowTopmost(!appIn->isWindowTopmost);
		} break;
		
		// +==============================+
		// |  AppCommand_ToggleClipSide   |
		// +==============================+
		case AppCommand_ToggleClipSide:
		{
			app->settings.clipNamesLeft = !app->settings.clipNamesLeft;
			SaveAppSettings();
		} break;
		
		// +==================================+
		// | AppCommand_ToggleSmoothScrolling |
		// +==================================+
		case AppCommand_ToggleSmoothScrolling:
		{
			app->settings.smoothScrollingDisabled = !app->settings.smoothScrollingDisabled;
			SaveAppSettings();
		} break;
		
		// +==================================+
		// | AppCommand_ToggleOptionTooltips  |
		// +==================================+
		case AppCommand_ToggleOptionTooltips:
		{
			app->settings.optionTooltipsDisabled = !app->settings.optionTooltipsDisabled;
			SaveAppSettings();
		} break;
		
		// +==============================+
		// |   AppCommand_ToggleTopbar    |
		// +==============================+
		case AppCommand_ToggleTopbar:
		{
			app->minimalModeEnabled = !app->minimalModeEnabled;
		} break;
		
		// +==============================+
		// |  AppCommand_ToggleSleeping   |
		// +==============================+
		case AppCommand_ToggleSleeping:
		{
			app->sleepingDisabled = !app->sleepingDisabled;
		} break;
		
		// +==================================+
		// | AppCommand_ToggleFrameIndicator  |
		// +==================================+
		case AppCommand_ToggleFrameIndicator:
		{
			app->enableFrameUpdateIndicator = !app->enableFrameUpdateIndicator;
		} break;
		
		// +==============================+
		// |  AppCommand_OpenCustomTheme  |
		// +==============================+
		case AppCommand_OpenCustomTheme:
		{
			if (app->openFileDialog.arena == nullptr)
			{
				#if (TARGET_IS_WINDOWS || TARGET_IS_LINUX)
				app->openFileDialogIsForTheme = true;
				OsDoOpenFileDialogAsync(stdHeap, true, &app->openFileDialog);
				Assert(app->openFileDialog.arena != nullptr || app->openFileDialog.error == Result_Canceled);
				#else //!(TARGET_IS_WINDOWS || TARGET_IS_LINUX)
				Notify_W("Open File dialog is not implemented on OSX currently! Please set the theme path in the settings.txt");
				#endif
			}
		} break;
		
		// +==============================+
		// | AppCommand_ClearCustomTheme  |
		// +==============================+
		case AppCommand_ClearCustomTheme:
		{
			if (!IsEmptyStr(app->settings.userThemePath))
			{
				SetAppSettingStr8Pntr(&app->settings, &app->settings.userThemePath, Str8_Empty);
				SaveAppSettings();
				AppLoadUserTheme();
				AppBakeTheme(false);
			}
		} break;
		
		// +================================+
		// | AppCommand_ToggleFileReloading |
		// +================================+
		case AppCommand_ToggleFileReloading:
		{
			app->settings.dontAutoReloadFile = !app->settings.dontAutoReloadFile;
			SaveAppSettings();
		} break;
		
		// +==============================+
		// |      AppCommand_NextTab      |
		// +==============================+
		case AppCommand_NextTab: //TODO: app->tabs.length > 1
		{
			if (app->tabs.length > 1) { AppChangeTab((app->currentTabIndex+1) % app->tabs.length); }
		} break;
		
		// +==============================+
		// |    AppCommand_PreviousTab    |
		// +==============================+
		case AppCommand_PreviousTab: //TODO: app->tabs.length > 1
		{
			if (app->tabs.length > 1) { AppChangeTab(app->currentTabIndex > 0 ? app->currentTabIndex-1 : app->tabs.length-1); }
		} break;
		
		// +==============================+
		// |   AppCommand_OpenFileMenu    |
		// +==============================+
		case AppCommand_OpenFileMenu:
		{
			app->isFileMenuOpen = !app->isFileMenuOpen;
			if (app->isFileMenuOpen)
			{
				app->isViewMenuOpen = false;
				app->keepFileMenuOpenUntilMouseOver = true;
			}
		} break;
		
		// +==============================+
		// |   AppCommand_OpenViewMenu    |
		// +==============================+
		case AppCommand_OpenViewMenu:
		{
			app->isViewMenuOpen = !app->isViewMenuOpen;
			if (app->isViewMenuOpen)
			{
				app->isFileMenuOpen = false;
				app->keepViewMenuOpenUntilMouseOver = true;
			}
		} break;
		
		// +==============================+
		// |   AppCommand_ResetUiScale    |
		// +==============================+
		case AppCommand_ResetUiScale:
		{
			app->uiFontSize = DEFAULT_UI_FONT_SIZE;
			app->mainFontSize = RoundR32(app->uiFontSize * MAIN_TO_UI_FONT_RATIO);
			app->settings.uiScale = 1.0f;
			bool fontBakeSuccess = AppCreateFonts();
			Assert(fontBakeSuccess);
			SaveAppSettings();
		} break;
		
		// +==============================+
		// |  AppCommand_IncreaseUiScale  |
		// +==============================+
		case AppCommand_IncreaseUiScale:
		{
			AppChangeFontSize(true);
		} break;
		
		// +==============================+
		// |  AppCommand_DecreaseUiScale  |
		// +==============================+
		case AppCommand_DecreaseUiScale:
		{
			AppChangeFontSize(false);
		} break;
		
		// +==============================+
		// |  AppCommand_TogglePerfGraph  |
		// +==============================+
		case AppCommand_TogglePerfGraph:
		{
			app->showPerfGraph = !app->showPerfGraph;
		} break;
		
		// +==============================+
		// |  AppCommand_ToggleClayDebug  |
		// +==============================+
		#if BUILD_WITH_CLAY
		case AppCommand_ToggleClayDebug:
		{
			Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
		} break;
		#endif //BUILD_WITH_CLAY
		
		// +==============================+
		// |    AppCommand_ScrollToTop    |
		// +==============================+
		case AppCommand_ScrollToTop: //TODO: app->currentTab != nullptr
		{
			#if BUILD_WITH_CLAY
			Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
			if (optionsListScrollData.found) { optionsListScrollData.scrollTarget->Y = 0; }
			#endif //BUILD_WITH_CLAY
		} break;
		
		// +==============================+
		// |  AppCommand_ScrollToBottom   |
		// +==============================+
		case AppCommand_ScrollToBottom: //TODO: app->currentTab != nullptr
		{
			#if BUILD_WITH_CLAY
			Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
			if (optionsListScrollData.found)
			{
				r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
				optionsListScrollData.scrollTarget->Y = -maxScroll;
			}
			#endif //BUILD_WITH_CLAY
		} break;
		
		// +==============================+
		// |   AppCommand_ScrollUpPage    |
		// +==============================+
		case AppCommand_ScrollUpPage: //TODO: app->currentTab != nullptr
		{
			#if BUILD_WITH_CLAY
			Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
			if (optionsListScrollData.found)
			{
				r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
				optionsListScrollData.scrollTarget->Y = ClampR32(optionsListScrollData.scrollTarget->Y + optionsListScrollData.scrollContainerDimensions.Height, -maxScroll, 0);
			}
			#endif //BUILD_WITH_CLAY
		} break;
		
		// +==============================+
		// |  AppCommand_ScrollDownPage   |
		// +==============================+
		case AppCommand_ScrollDownPage: //TODO: app->currentTab != nullptr
		{
			#if BUILD_WITH_CLAY
			Clay_ScrollContainerData optionsListScrollData = Clay_GetScrollContainerData(CLAY_ID("OptionsList"), false);
			if (optionsListScrollData.found)
			{
				r32 maxScroll = MaxR32(0, optionsListScrollData.contentDimensions.Height - optionsListScrollData.scrollContainerDimensions.Height);
				optionsListScrollData.scrollTarget->Y = ClampR32(optionsListScrollData.scrollTarget->Y - optionsListScrollData.scrollContainerDimensions.Height, -maxScroll, 0);
			}
			#endif //BUILD_WITH_CLAY
		} break;
		
		#if BUILD_WITH_CLAY
		#define CALC_SMALL_OPTION_GRID_SPECS(optionsAreaWidthVarName, buttonMarginVarName, buttonWidthVarName, numColumnsVarName, numRowsVarName)           \
			r32 optionsAreaWidthVarName = (r32)appIn->screenSize.Width - (app->minimalModeEnabled ? 0.0f : UI_R32(SCROLLBAR_WIDTH)) - (r32)(UI_U16(4) * 2); \
			u16 buttonMarginVarName = UI_U16(SMALL_BTN_MARGIN);                                                                                             \
			r32 buttonWidthVarName = app->currentTab->longestAbbreviationWidth + (r32)UI_U16(SMALL_BTN_PADDING_X)*2;                                        \
			i32 numColumnsVarName = FloorR32i((optionsAreaWidthVarName - (r32)buttonMarginVarName) / (buttonWidthVarName + (r32)buttonMarginVarName));      \
			if (numColumnsVarName <= 0) { numColumnsVarName = 1; }                                                                                          \
			i32 numRowsVarName = CeilDivI32((i32)app->currentTab->fileOptions.length, numColumnsVarName)
		#endif // BUILD_WITH_CLAY
		
		// +==============================+
		// |   AppCommand_SelectMoveUp    |
		// +==============================+
		case AppCommand_SelectMoveUp: //TODO: app->currentTab != nullptr
		{
			app->usingKeyboardToSelect = true;
			if (app->currentTab != nullptr)
			{
				if (app->currentTab->selectedOptionIndex == -1 && app->currentTab->fileOptions.length > 0)
				{
					SelectAnOption();
					#if BUILD_WITH_CLAY
					AutoScrollToSelectedOptionAfterMove();
					#elif BUILD_WITH_PIG_UI
					app->scrollToSelectedOption = true;
					#endif //BUILD_WITH_CLAY
				}
				else if (app->currentTab->selectedOptionIndex >= 0)
				{
					if (app->settings.smallButtons)
					{
						#if BUILD_WITH_CLAY
						CALC_SMALL_OPTION_GRID_SPECS(optionsAreaWidth, buttonMargin, buttonWidth, numColumns, numRows);
						UNUSED(numRows);
						
						if (app->currentTab->selectedOptionIndex >= numColumns)
						{
							app->currentTab->selectedOptionIndex -= numColumns;
							AutoScrollToSelectedOptionAfterMove();
						}
						else if (true /*upPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = (ixx)(app->currentTab->fileOptions.length-1) - ((ixx)numColumns - app->currentTab->selectedOptionIndex);
							AutoScrollToSelectedOptionAfterMove();
						}
						#elif BUILD_WITH_PIG_UI
						if (app->currentTab->selectedOptionIndex >= (ixx)app->smallBtnNumColumns)
						{
							app->currentTab->selectedOptionIndex -= app->smallBtnNumColumns;
							app->scrollToSelectedOption = true;
						}
						else if (true /*upPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = (ixx)(app->currentTab->fileOptions.length-1) - ((ixx)app->smallBtnNumColumns - app->currentTab->selectedOptionIndex);
							app->scrollToSelectedOption = true;
						}
						#endif //BUILD_WITH_CLAY
					}
					else
					{
						if (app->currentTab->selectedOptionIndex > 0)
						{
							app->currentTab->selectedOptionIndex--;
							#if BUILD_WITH_CLAY
							AutoScrollToSelectedOptionAfterMove();
							#elif BUILD_WITH_PIG_UI
							app->scrollToSelectedOption = true;
							#endif //BUILD_WITH_CLAY
						}
						else if (true /*upPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							#if BUILD_WITH_CLAY
							AutoScrollToSelectedOptionAfterMove();
							#elif BUILD_WITH_PIG_UI
							app->scrollToSelectedOption = true;
							#endif //BUILD_WITH_CLAY
						}
					}
				}
			}
		} break;
		
		// +==============================+
		// |  AppCommand_SelectMoveDown   |
		// +==============================+
		case AppCommand_SelectMoveDown: //TODO: app->currentTab != nullptr
		{
			app->usingKeyboardToSelect = true;
			if (app->currentTab != nullptr)
			{
				if (app->currentTab->selectedOptionIndex == -1 && app->currentTab->fileOptions.length > 0)
				{
					SelectAnOption();
					#if BUILD_WITH_CLAY
					AutoScrollToSelectedOptionAfterMove();
					#elif BUILD_WITH_PIG_UI
					app->scrollToSelectedOption = true;
					#endif //BUILD_WITH_CLAY
				}
				else if (app->currentTab->selectedOptionIndex >= 0)
				{
					if (app->settings.smallButtons)
					{
						#if BUILD_WITH_CLAY
						CALC_SMALL_OPTION_GRID_SPECS(optionsAreaWidth, buttonMargin, buttonWidth, numColumns, numRows);
						
						if (((i32)app->currentTab->selectedOptionIndex / numColumns) < numRows-1)
						{
							app->currentTab->selectedOptionIndex += numColumns;
							if ((uxx)app->currentTab->selectedOptionIndex >= app->currentTab->fileOptions.length)
							{
								app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							}
							AutoScrollToSelectedOptionAfterMove();
						}
						else if (true /*downPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = (app->currentTab->selectedOptionIndex + numColumns) % (ixx)app->currentTab->fileOptions.length;
							AutoScrollToSelectedOptionAfterMove();
						}
						#elif BUILD_WITH_PIG_UI
						if (((i32)app->currentTab->selectedOptionIndex / app->smallBtnNumColumns) < app->smallBtnNumRows-1)
						{
							app->currentTab->selectedOptionIndex += app->smallBtnNumColumns;
							if ((uxx)app->currentTab->selectedOptionIndex >= app->currentTab->fileOptions.length)
							{
								app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
							}
							app->scrollToSelectedOption = true;
						}
						else if (true /*downPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = (app->currentTab->selectedOptionIndex + app->smallBtnNumColumns) % (ixx)app->currentTab->fileOptions.length;
							app->scrollToSelectedOption = true;
						}
						#endif //BUILD_WITH_CLAY
					}
					else
					{
						if ((uxx)app->currentTab->selectedOptionIndex < app->currentTab->fileOptions.length-1)
						{
							app->currentTab->selectedOptionIndex++;
							#if BUILD_WITH_CLAY
							AutoScrollToSelectedOptionAfterMove();
							#elif BUILD_WITH_PIG_UI
							app->scrollToSelectedOption = true;
							#endif //BUILD_WITH_CLAY
						}
						else if (true /*downPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
						{
							app->currentTab->selectedOptionIndex = 0;
							#if BUILD_WITH_CLAY
							AutoScrollToSelectedOptionAfterMove();
							#elif BUILD_WITH_PIG_UI
							app->scrollToSelectedOption = true;
							#endif //BUILD_WITH_CLAY
						}
					}
				}
			}
		} break;
		
		// +==============================+
		// |  AppCommand_SelectMoveLeft   |
		// +==============================+
		case AppCommand_SelectMoveLeft: //TODO: app->currentTab != nullptr
		{
			app->usingKeyboardToSelect = true;
			if (app->currentTab != nullptr)
			{
				if (app->currentTab->selectedOptionIndex == -1 && app->currentTab->fileOptions.length > 0)
				{
					SelectAnOption();
					#if BUILD_WITH_CLAY
					AutoScrollToSelectedOptionAfterMove();
					#elif BUILD_WITH_PIG_UI
					app->scrollToSelectedOption = true;
					#endif //BUILD_WITH_CLAY
				}
				else if (app->currentTab->selectedOptionIndex >= 0 && app->settings.smallButtons)
				{
					if (app->currentTab->selectedOptionIndex > 0)
					{
						app->currentTab->selectedOptionIndex--;
						#if BUILD_WITH_CLAY
						AutoScrollToSelectedOptionAfterMove();
						#elif BUILD_WITH_PIG_UI
						app->scrollToSelectedOption = true;
						#endif //BUILD_WITH_CLAY
					}
					else if (true /*leftPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
					{
						app->currentTab->selectedOptionIndex = (ixx)app->currentTab->fileOptions.length-1;
						#if BUILD_WITH_CLAY
						AutoScrollToSelectedOptionAfterMove();
						#elif BUILD_WITH_PIG_UI
						app->scrollToSelectedOption = true;
						#endif //BUILD_WITH_CLAY
					}
				}
			}
		} break;
		
		// +==============================+
		// |  AppCommand_SelectMoveRight  |
		// +==============================+
		case AppCommand_SelectMoveRight: //TODO: app->currentTab != nullptr
		{
			app->usingKeyboardToSelect = true;
			if (app->currentTab != nullptr)
			{
				if (app->currentTab->selectedOptionIndex == -1 && app->currentTab->fileOptions.length > 0)
				{
					SelectAnOption();
					#if BUILD_WITH_CLAY
					AutoScrollToSelectedOptionAfterMove();
					#elif BUILD_WITH_PIG_UI
					app->scrollToSelectedOption = true;
					#endif //BUILD_WITH_CLAY
				}
				else if (app->currentTab->selectedOptionIndex >= 0 && app->settings.smallButtons)
				{
					if ((uxx)app->currentTab->selectedOptionIndex < app->currentTab->fileOptions.length-1)
					{
						app->currentTab->selectedOptionIndex++;
						#if BUILD_WITH_CLAY
						AutoScrollToSelectedOptionAfterMove();
						#elif BUILD_WITH_PIG_UI
						app->scrollToSelectedOption = true;
						#endif //BUILD_WITH_CLAY
					}
					else if (true /*rightPressed*/) //TODO: Re-enable this distinction somehow? We only wanna loop around if the key was pressed, not when it was held down and OS-level repeated
					{
						app->currentTab->selectedOptionIndex = 0;
						#if BUILD_WITH_CLAY
						AutoScrollToSelectedOptionAfterMove();
						#elif BUILD_WITH_PIG_UI
						app->scrollToSelectedOption = true;
						#endif //BUILD_WITH_CLAY
					}
				}
			}
		} break;
		
		// +==============================+
		// |  AppCommand_ToggleSelected   |
		// +==============================+
		case AppCommand_ToggleSelected: //TODO: app->usingKeyboardToSelect && app->currentTab != nullptr && app->currentTab->selectedOptionIndex >= 0
		{
			if (app->currentTab != nullptr && app->currentTab->selectedOptionIndex >= 0)
			{
				FileOption* selectedOption = VarArrayGetHard(FileOption, &app->currentTab->fileOptions, (uxx)app->currentTab->selectedOptionIndex);
				if (selectedOption->type == FileOptionType_Bool)
				{
					ToggleOption(app->currentTab, selectedOption);
				}
			}
		} break;
		
		// +==============================+
		// | AppCommand_ClearRecentFiles  |
		// +==============================+
		case AppCommand_ClearRecentFiles:
		{
			if (app->recentFiles.length > 0 && !app->popup.isOpen)
			{
				OpenPopupDialog(stdHeap, &app->popup,
					ScratchPrintStr("Are you sure you want to clear %s%llu recent file entr%s?", (app->recentFiles.length > 1) ? "all " : "", app->recentFiles.length, PluralEx(app->recentFiles.length, "y", "ies")),
					AppClearRecentFilesPopupCallback, nullptr
				);
				AddPopupButton(&app->popup, 1, StrLit("Cancel"), PopupDialogResult_No, GetThemeColor(ConfirmDialogNeutralBtnBorder));
				AddPopupButton(&app->popup, 2, StrLit("Clear Recent Files"), PopupDialogResult_Yes, GetThemeColor(ConfirmDialogNegativeBtnBorder));
			}
		} break;
		
		default:
		{
			DebugAssertMsg(false, "Unimplemented AppCommand!");
		} break;
	}
	
}
