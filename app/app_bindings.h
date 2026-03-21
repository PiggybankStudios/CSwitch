/*
File:   app_bindings.h
Author: Taylor Robbins
Date:   02\01\2026
*/

#ifndef _APP_BINDINGS_H
#define _APP_BINDINGS_H

//TODO: Add support for mouse bindings?

#define APP_BINDING_ID_INVALID 0

typedef plex AppBinding AppBinding;
plex AppBinding
{
	uxx id;
	u8 modifierKeys;
	Key key;
	AppCommand command;
};

typedef plex AppBindingSet AppBindingSet;
plex AppBindingSet
{
	Arena* arena;
	uxx nextBindingId;
	VarArray bindings; //AppBinding
	bool isKeyModifierSensitive[Key_Count];
};

#endif //  _APP_BINDINGS_H
