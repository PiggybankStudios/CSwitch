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

AppBinding* FindBindingForAppCommand(AppBindingSet* bindings, AppCommand command, uxx skipCount)
{
	NotNull(bindings);
	NotNull(bindings->arena);
	uxx foundIndex = 0;
	VarArrayLoop(&bindings->bindings, bIndex)
	{
		VarArrayLoopGet(AppBinding, binding, &bindings->bindings, bIndex);
		if (binding->command == command)
		{
			if (foundIndex >= skipCount) { return binding; }
			foundIndex++;
		}
	}
	return nullptr;
}
Str8 GetBindingStrForAppCommand(AppBindingSet* bindings, AppCommand command, Arena* arena, uxx skipCount)
{
	AppBinding* binding = FindBindingForAppCommand(bindings, command, skipCount);
	if (binding == nullptr) { return Str8_Empty; }
	bool controlMod = IsFlagSet(binding->modifierKeys, ModifierKey_Control);
	bool altMod     = IsFlagSet(binding->modifierKeys, ModifierKey_Alt);
	bool shiftMod   = IsFlagSet(binding->modifierKeys, ModifierKey_Shift);
	Str8 result = PrintInArenaStr(arena, "%s%s%s%s%s%s%s",
		controlMod ? "Ctrl" : "",
		(controlMod && altMod) ? "+" : "",
		altMod ? "Alt" : "",
		((controlMod || altMod) && shiftMod) ? "+" : "",
		shiftMod ? "Shift" : "",
		(controlMod || altMod || shiftMod) ? "+" : "",
		GetKeyStr(binding->key)
	);
	return result;
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

Result TryParseBindingFile(Str8 fileContents, AppBindingSet* bindingsOut)
{
	NotNullStr(fileContents);
	NotNull(bindingsOut);
	AssertMsg(bindingsOut->arena != nullptr, "Must initialized AppBindingSet before passing to TryParseBindingFile");
	
	uxx numValidBindingsFound = 0;
	bool foundInvalidBindings = false;
	bool foundInvalidSyntax = false;
	Result result = Result_None;
	TextParser parser = MakeTextParser(fileContents);
	ParsingToken token = ZEROED;
	while (TextParserGetToken(&parser, &token))
	{
		switch (token.type)
		{
			case ParsingTokenType_Comment: /* do nothing */ break;
			
			case ParsingTokenType_KeyValuePair:
			{
				Str8 keyName = token.key;
				uxx previousPlusIndex = 0;
				u8 modifierKeys = ModifierKey_None;
				bool allModifiersValid = true;
				for (uxx cIndex = 0; cIndex < token.key.length; cIndex++)
				{
					if (token.key.chars[cIndex] == '+')
					{
						Str8 modifierName = TrimWhitespace(StrSlice(token.key, previousPlusIndex, cIndex));
						if (StrAnyCaseEquals(modifierName, StrLit("Control")) || StrAnyCaseEquals(modifierName, StrLit("Ctrl")) || StrAnyCaseEquals(modifierName, StrLit("Ctl")))
						{
							if (IsFlagSet(modifierKeys, ModifierKey_Control)) { NotifyPrint_W("Duplicated Ctrl modifier in bindings file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.key)); }
							modifierKeys |= ModifierKey_Control;
						}
						else if (StrAnyCaseEquals(modifierName, StrLit("Alt"))) //TODO: "Super" or "Command"?
						{
							if (IsFlagSet(modifierKeys, ModifierKey_Alt)) { NotifyPrint_W("Duplicated Alt modifier in bindings file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.key)); }
							modifierKeys |= ModifierKey_Alt;
						}
						else if (StrAnyCaseEquals(modifierName, StrLit("Shift"))) //TODO: "Shft" or some other abbreviation?
						{
							if (IsFlagSet(modifierKeys, ModifierKey_Shift)) { NotifyPrint_W("Duplicated Shift modifier in bindings file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.key)); }
							modifierKeys |= ModifierKey_Shift;
						}
						else if (IsEmptyStr(modifierName))
						{
							NotifyPrint_W("Empty modifier part in bindings file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.key));
							allModifiersValid = false;
							foundInvalidBindings = true;
							break;
						}
						else
						{
							NotifyPrint_W("Unknown modifier \"%.*s\" in bindings file on line %llu: \"%.*s\"", StrPrint(modifierName), parser.lineParser.lineIndex, StrPrint(token.key));
							allModifiersValid = false;
							foundInvalidBindings = true;
							break;
						}
						keyName = StrSliceFrom(token.key, cIndex+1);
						previousPlusIndex = cIndex+1;
					}
				}
				
				keyName = TrimWhitespace(keyName);
				
				if (IsEmptyStr(keyName))
				{
					NotifyPrint_W("Modifiers only in bindings file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.key));
					foundInvalidBindings = true;
				}
				else if (allModifiersValid)
				{
					Key key = Key_None;
					for (uxx keyIndex = 1; keyIndex < Key_Count; keyIndex++)
					{
						Key keyEnumValue = (Key)keyIndex;
						Str8 keyEnumName = MakeStr8Nt(GetKeyStr(keyEnumValue));
						if (StrAnyCaseEquals(keyName, keyEnumName)) { key = keyEnumValue; break; }
					}
					
					if (key == Key_None)
					{
						NotifyPrint_W("Unknown key name \"%.*s\" in bindings file on line %llu: \"%.*s\"", StrPrint(keyName), parser.lineParser.lineIndex, StrPrint(token.key));
					}
					else
					{
						AppCommand command = AppCommand_None;
						for (uxx cIndex = 1; cIndex < AppCommand_Count; cIndex++)
						{
							AppCommand commandEnumValue = (AppCommand)cIndex;
							Str8 commandEnumName = MakeStr8Nt(GetAppCommandStr(commandEnumValue));
							if (StrAnyCaseEquals(token.value, commandEnumName))
							{
								command = commandEnumValue;
								break;
							}
						}
						
						if (StrAnyCaseEquals(token.value, StrLit("None")))
						{
							RemoveAppBinding(bindingsOut, modifierKeys, key);
							numValidBindingsFound++;
						}
						else if (command == AppCommand_None)
						{
							NotifyPrint_W("Unknown command name \"%.*s\" in bindings file on line %llu: \"%.*s\"", StrPrint(token.value), parser.lineParser.lineIndex, StrPrint(token.str));
						}
						else
						{
							AddAppBinding(bindingsOut, modifierKeys, key, command);
							numValidBindingsFound++;
						}
					}
				}
			} break;
			
			default:
			{
				PrintLine_W("Invalid syntax in binding file on line %llu: \"%.*s\"", parser.lineParser.lineIndex, StrPrint(token.str));
				foundInvalidSyntax = true;
			}  break;
		}
	}
	
	if (numValidBindingsFound == 0 && result == Result_None)
	{
		if (foundInvalidBindings || foundInvalidSyntax) { Notify_E("All bindings in file were invalid!"); result = Result_InvalidSyntax; }
		else { PrintLine_W("Bindings file contained no bindings!"); result = Result_EmptyFile; }
	}
	
	if (result == Result_None) { result = Result_Success; }
	return result;
}

Str8 SerializeCommentedBindingsFileFromBindingSet(Arena* arena, AppBindingSet* defaultBindingSet)
{
	TwoPassStr8Loop(result, arena, false)
	{
		TwoPassStrNt(&result, "// This file stores all the user configured bindings for functionality in C-Switch.\n");
		TwoPassStrNt(&result, "// The following commented lines are a copy of the default bindings that are baked into the program.\n");
		TwoPassStrNt(&result, "// Override these bindings with your own or unbind a key combo by assigning it to \"None\".\n");
		TwoPassStrNt(&result, "// Key combos are modifier keys (Ctrl, Alt, and Shift) joined by \'+\' character ending in a single valid key name like \"Enter\".\n");
		TwoPassStrNt(&result, "// Whitespace is allowed pretty much anywhere you would expect. Comments are allowed with // but multi-line comment syntax like /* */ is not allowed.\n");
		
		TwoPassStrNt(&result, "// \n");
		
		TwoPassStrNt(&result, "// Valid Key Names: ");
		for (uxx keyIndex = 1; keyIndex < Key_Count; keyIndex++)
		{
			Key key = (Key)keyIndex;
			if (key != Key_Control && key != Key_Alt && key != Key_Shift &&
				key != Key_Windows && key != Key_Command && key != Key_Option &&
				key != Key_LeftShift && key != Key_LeftControl && key != Key_LeftAlt &&
				key != Key_LeftWindows && key != Key_LeftCommand && key != Key_LeftOption &&
				key != Key_RightShift && key != Key_RightControl && key != Key_RightAlt &&
				key != Key_RightWindows && key != Key_RightCommand && key != Key_RightOption)
			{
				if (keyIndex > 1) { TwoPassStrNt(&result, ", "); }
				TwoPassStrNt(&result, GetKeyStr(key));
			}
		}
		TwoPassStrNt(&result, "\n");
		
		TwoPassStrNt(&result, "// \n");
		
		TwoPassStrNt(&result, "// Bindable Commands: ");
		for (uxx cIndex = 1; cIndex < AppCommand_Count; cIndex++)
		{
			AppCommand appCommand = (AppCommand)cIndex;
			if (cIndex > 1) { TwoPassStrNt(&result, ", "); }
			TwoPassStrNt(&result, GetAppCommandStr(appCommand));
		}
		TwoPassStrNt(&result, "\n");
		
		TwoPassStrNt(&result, "\n");
		
		TwoPassStrNt(&result, "// Change one of these bindings by uncommenting it and setting it to something else, or \"None\" to remove the default binding:\n");
		VarArrayLoop(&defaultBindingSet->bindings, bIndex)
		{
			VarArrayLoopGet(AppBinding, binding, &defaultBindingSet->bindings, bIndex);
			if (binding->id != APP_BINDING_ID_INVALID)
			{
				TwoPassStrNt(&result, "// ");
				bool controlMod = IsFlagSet(binding->modifierKeys, ModifierKey_Control);
				bool altMod     = IsFlagSet(binding->modifierKeys, ModifierKey_Alt);
				bool shiftMod   = IsFlagSet(binding->modifierKeys, ModifierKey_Shift);
				if (controlMod)                         { TwoPassStrNt(&result, "Ctrl");  }
				if (controlMod && altMod)               { TwoPassStrNt(&result, "+");     }
				if (altMod)                             { TwoPassStrNt(&result, "Alt");   }
				if ((controlMod || altMod) && shiftMod) { TwoPassStrNt(&result, "+");     }
				if (shiftMod)                           { TwoPassStrNt(&result, "Shift"); }
				if (controlMod || altMod || shiftMod)   { TwoPassStrNt(&result, "+");     }
				
				TwoPassStrNt(&result, GetKeyStr(binding->key));
				TwoPassStrNt(&result, ": ");
				TwoPassStrNt(&result, GetAppCommandStr(binding->command));
				TwoPassStrNt(&result, "\n");
			}
		}
		
		TwoPassStrNt(&result, "\n");
		
		TwoPassStrNt(&result, "// Add new bindings here:\n");
		
		TwoPassStr8LoopEnd(&result);
	}
	return result.str;
}
