/*
File:   app_bindings.c
Author: Taylor Robbins
Date:   02\01\2026
Description: 
	** Bindings are created between a key press (optionally with modifiers like Control, Alt, Shift) and an AppCommand
	** When that particular key is pressed the AppCommand will run.
	** Bindings can be deserialized from an external file so they can be edited by the user
	** NOTE: Currently bindings only support global and unparamaterized commands. And they only support keyboard keys, no mouse clicks or touchscreen gestures
*/

void FreeAppBindingSet(AppBindingSet* bindings)
{
	NotNull(bindings);
	if (bindings->arena != nullptr)
	{
		FreeVarArray(&bindings->bindings);
	}
	ClearPointer(bindings);
}

void InitAppBindingSet(Arena* arena, AppBindingSet* bindingsOut)
{
	ClearPointer(bindingsOut);
	bindingsOut->arena = arena;
	bindingsOut->nextBindingId = 1;
	InitVarArrayWithInitial(AppBinding, &bindingsOut->bindings, arena, AppCommand_Count);
}

AppBinding* FindAppBindingById(AppBindingSet* bindings, uxx bindingId)
{
	NotNull(bindings);
	NotNull(bindings->arena);
	if (bindingId == APP_BINDING_ID_INVALID) { return nullptr; }
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->id == bindingId) { return binding; }
	}
	return nullptr;
}

AppBinding* FindAppBinding(AppBindingSet* bindings, u8 modifierKeys, Key key)
{
	NotNull(bindings);
	NotNull(bindings->arena);
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->id != APP_BINDING_ID_INVALID && binding->modifierKeys == modifierKeys && binding->key == key)
		{
			return binding;
		}
	}
	return nullptr;
}
AppCommand FindAppBindingCommand(AppBindingSet* bindings, u8 modifierKeys, Key key)
{
	AppBinding* binding = FindAppBinding(bindings, modifierKeys, key);
	return (binding != nullptr) ? binding->command : AppCommand_None;
}

void RemoveAppBinding(AppBindingSet* bindings, u8 modifierKeys, Key key)
{
	bool foundBindingWithSameKeyButDifferentMods = false;
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->id != APP_BINDING_ID_INVALID)
		{
			if (binding->modifierKeys == modifierKeys && binding->key == key)
			{
				binding->id = APP_BINDING_ID_INVALID;
			}
			else if (binding->key == key) { foundBindingWithSameKeyButDifferentMods = true; }
		}
	}
	
	bindings->isKeyModifierSensitive[key] = foundBindingWithSameKeyButDifferentMods;
}

AppBinding* AddAppBinding(AppBindingSet* bindings, u8 modifierKeys, Key key, AppCommand command)
{
	NotNull(bindings);
	NotNull(bindings->arena);
	AppBinding* existingOrEmptyBinding = nullptr;
	bool foundBindingWithSameKeyButDifferentMods = false;
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->id == APP_BINDING_ID_INVALID)
		{
			if (existingOrEmptyBinding == nullptr)
			{
				existingOrEmptyBinding = binding;
			}
		}
		else if (binding->modifierKeys == modifierKeys && binding->key == key)
		{
			existingOrEmptyBinding = binding;
		}
		else if (binding->key == key) { foundBindingWithSameKeyButDifferentMods = true; }
	}
	
	AppBinding* result = nullptr;
	if (existingOrEmptyBinding != nullptr)
	{
		result = existingOrEmptyBinding;
		if (result->id == APP_BINDING_ID_INVALID)
		{
			ClearPointer(result);
			result->id = bindings->nextBindingId;
			bindings->nextBindingId++;
		}
	}
	else
	{
		result = VarArrayAdd(AppBinding, &bindings->bindings);
		NotNull(result);
		ClearPointer(result);
		result->id = bindings->nextBindingId;
		bindings->nextBindingId++;
	}
	
	result->modifierKeys = modifierKeys;
	result->key = key;
	result->command = command;
	
	bindings->isKeyModifierSensitive[key] = foundBindingWithSameKeyButDifferentMods;
	
	return result;
}

AppBinding* AddAppBindingRef(AppBindingSet* bindings, const AppBinding* bindingPntr)
{
	return AddAppBinding(bindings, bindingPntr->modifierKeys, bindingPntr->key, bindingPntr->command);
}

void AddDefaultAppBindings(AppBindingSet* bindings)
{
	#define NoMods ModifierKey_None
	#define Ctrl   ModifierKey_Control
	#define Alt    ModifierKey_Alt
	#define Shift  ModifierKey_Shift
	
	AddAppBinding(bindings, Ctrl,       Key_W,              AppCommand_CloseTab); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, Ctrl|Shift, Key_W,              AppCommand_CloseWindow);
	AddAppBinding(bindings, NoMods,     Key_F6,             AppCommand_TogglePerfGraph);
	AddAppBinding(bindings, NoMods,     Key_F9,             AppCommand_ToggleLightMode);
	AddAppBinding(bindings, NoMods,     Key_F10,            AppCommand_ToggleSmallButtons);
	AddAppBinding(bindings, NoMods,     Key_F11,            AppCommand_ToggleTopbar);
	AddAppBinding(bindings, NoMods,     Key_Escape,         AppCommand_ClosePopupOrMenu);
	AddAppBinding(bindings, Ctrl,       Key_Plus,           AppCommand_IncreaseUiScale);
	AddAppBinding(bindings, Ctrl,       Key_NumpadAdd,      AppCommand_IncreaseUiScale);
	AddAppBinding(bindings, Ctrl,       Key_Minus,          AppCommand_DecreaseUiScale);
	AddAppBinding(bindings, Ctrl,       Key_NumpadSubtract, AppCommand_DecreaseUiScale);
	AddAppBinding(bindings, Ctrl,       Key_0,              AppCommand_ResetUiScale);
	AddAppBinding(bindings, Alt,        Key_F,              AppCommand_OpenFileMenu);
	AddAppBinding(bindings, Alt,        Key_V,              AppCommand_OpenViewMenu);
	AddAppBinding(bindings, Ctrl,       Key_O,              AppCommand_OpenFile);
	AddAppBinding(bindings, Ctrl,       Key_Tab,            AppCommand_NextTab); //TODO: app->tabs.length > 1
	AddAppBinding(bindings, Ctrl|Alt,   Key_X,              AppCommand_NextTab); //TODO: app->tabs.length > 1
	AddAppBinding(bindings, Ctrl|Shift, Key_Tab,            AppCommand_PreviousTab); //TODO: app->tabs.length > 1
	AddAppBinding(bindings, Ctrl|Alt,   Key_Z,              AppCommand_PreviousTab); //TODO: app->tabs.length > 1
	AddAppBinding(bindings, Ctrl,       Key_E,              AppCommand_ReopenRecentFile);
	AddAppBinding(bindings, Ctrl,       Key_T,              AppCommand_ToggleTopmost);
	AddAppBinding(bindings, NoMods,     Key_Home,           AppCommand_ScrollToTop); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_End,            AppCommand_ScrollToBottom); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_PageUp,         AppCommand_ScrollUpPage); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_PageDown,       AppCommand_ScrollDownPage); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_Up,             AppCommand_SelectMoveUp); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_Down,           AppCommand_SelectMoveDown); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_Left,           AppCommand_SelectMoveLeft); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_Right,          AppCommand_SelectMoveRight); //TODO: app->currentTab != nullptr
	AddAppBinding(bindings, NoMods,     Key_Enter,          AppCommand_ToggleSelected); //TODO: app->usingKeyboardToSelect && app->currentTab != nullptr && app->currentTab->selectedOptionIndex >= 0
	
	#if DEBUG_BUILD
	AddAppBinding(bindings, NoMods,     Key_Tilde,  AppCommand_ToggleClayDebug);
	#endif
	
	#undef NoMods
	#undef Ctrl
	#undef Alt
	#undef Shift
}

void RunAppBindingCommands(AppBindingSet* bindings)
{
	if (!appIn->isFocused) { return; }
	
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->id != APP_BINDING_ID_INVALID)
		{
			bool runCommand = false;
			bool followOsRepeatedEvent = DoesAppCommandTriggerOnOsLevelKeyRepeatEvents(binding->command);
			//TODO: We need to somehow know if the key press was an OS-level repeat press and pass that info on to RunAppCommand for stuff like AppCommand_SelectMoveDown to not loop around when holding the Down arrow key
			
			if (bindings->isKeyModifierSensitive[binding->key] || (binding->modifierKeys != ModifierKey_None && binding->modifierKeys != ModifierKey_Any))
			{
				if (WasKeyComboPressed(binding->modifierKeys, binding->key, followOsRepeatedEvent))
				{
					if (!followOsRepeatedEvent) { appInputHandling->keyboard.keyHandledUntilReleased[binding->key] = true; } //TODO: Enable this always once we figure out how we prevent suppressing key repeated frames
					runCommand = true;
				}
			}
			else
			{
				if (WasKeyPressed(binding->key, followOsRepeatedEvent))
				{
					if (!followOsRepeatedEvent) { appInputHandling->keyboard.keyHandledUntilReleased[binding->key] = true; } //TODO: Enable this always once we figure out how we prevent suppressing key repeated frames
					runCommand = true;
				}
			}
			
			if (runCommand)
			{
				#if DEBUG_BUILD
				bool controlMod = IsFlagSet(binding->modifierKeys, ModifierKey_Control);
				bool altMod     = IsFlagSet(binding->modifierKeys, ModifierKey_Alt);
				bool shiftMod   = IsFlagSet(binding->modifierKeys, ModifierKey_Shift);
				PrintLine_O("%s%s%s%s%s%s%s -> AppCommand_%s",
					controlMod ? "Ctrl" : "",
					(controlMod && altMod) ? "+" : "",
					altMod ? "Alt" : "",
					((controlMod || altMod) && shiftMod) ? "+" : "",
					shiftMod ? "Shift" : "",
					(controlMod || altMod || shiftMod) ? "+" : "",
					GetKeyStr(binding->key),
					GetAppCommandStr(binding->command)
				);
				#endif
				
				RunAppCommand(binding->command);
			}
		}
	}
}
