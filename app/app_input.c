/*
File:   app_input.c
Author: Taylor Robbins
Date:   01\31\2026
Description:
	** This holds aliases for all the input functions (IsKeyboardDown, IsMousePressed, etc.)
	** that automatically use appIn and appInputHandling global variables defined in app_main.c.
	** AppInput structure is defined in platform_interface.h since it's needed by both the
	** application and the platform layer
*/

#define AppInGlobalIsSet() (appIn != nullptr && appInputHandling != nullptr)

#define IsKeyDown(key)                                             (AppInGlobalIsSet() && IsKeyboardKeyDown(&appIn->keyboard,                 &appInputHandling->keyboard, (key)))
#define IsKeyUp(key)                                               (AppInGlobalIsSet() && IsKeyboardKeyUp(&appIn->keyboard,                   &appInputHandling->keyboard, (key)))
#define WasKeyPressed(key, followOsRepeatingEvent)                 (AppInGlobalIsSet() && IsKeyboardKeyPressed(&appIn->keyboard,              &appInputHandling->keyboard, (key), (followOsRepeatingEvent)))
#define WasKeyReleased(key)                                        (AppInGlobalIsSet() && IsKeyboardKeyReleased(&appIn->keyboard,             &appInputHandling->keyboard, (key)))
#define WasKeyComboPressed(modifiers, key, followOsRepeatingEvent) (AppInGlobalIsSet() && IsKeyboardKeyPressedWithModifiers(&appIn->keyboard, &appInputHandling->keyboard, (modifiers), (key), (followOsRepeatingEvent)))

#define IsKeyDownRaw(key)                                             (AppInGlobalIsSet() && IsKeyboardKeyDown(&appIn->keyboard,                 nullptr, (key)))
#define IsKeyUpRaw(key)                                               (AppInGlobalIsSet() && IsKeyboardKeyUp(&appIn->keyboard,                   nullptr, (key)))
#define WasKeyPressedRaw(key, followOsRepeatingEvent)                 (AppInGlobalIsSet() && IsKeyboardKeyPressed(&appIn->keyboard,              nullptr, (key), (followOsRepeatingEvent)))
#define WasKeyReleasedRaw(key)                                        (AppInGlobalIsSet() && IsKeyboardKeyReleased(&appIn->keyboard,             nullptr, (key)))
#define WasKeyComboPressedRaw(modifiers, key, followOsRepeatingEvent) (AppInGlobalIsSet() && IsKeyboardKeyPressedWithModifiers(&appIn->keyboard, nullptr, (modifiers), (key), (followOsRepeatingEvent)))

#define IsMouseDown(btn)      (AppInGlobalIsSet() && IsMouseBtnDown(&appIn->mouse,     &appInputHandling->mouse, (btn)))
#define IsMouseUp(btn)        (AppInGlobalIsSet() && IsMouseBtnUp(&appIn->mouse,       &appInputHandling->mouse, (btn)))
#define WasMousePressed(btn)  (AppInGlobalIsSet() && IsMouseBtnPressed(&appIn->mouse,  &appInputHandling->mouse, (btn)))
#define WasMouseReleased(btn) (AppInGlobalIsSet() && IsMouseBtnReleased(&appIn->mouse, &appInputHandling->mouse, (btn)))
#define MouseLeftClicked()    WasMousePressed(MouseBtn_Left)
#define MouseRightClicked()   WasMousePressed(MouseBtn_Right)
#define MouseMiddleClicked()  WasMousePressed(MouseBtn_Middle)

#define IsMouseDownRaw(btn)      (AppInGlobalIsSet() && IsMouseBtnDown(&appIn->mouse,     nullptr, (btn)))
#define IsMouseUpRaw(btn)        (AppInGlobalIsSet() && IsMouseBtnUp(&appIn->mouse,       nullptr, (btn)))
#define WasMousePressedRaw(btn)  (AppInGlobalIsSet() && IsMouseBtnPressed(&appIn->mouse,  nullptr, (btn)))
#define WasMouseReleasedRaw(btn) (AppInGlobalIsSet() && IsMouseBtnReleased(&appIn->mouse, nullptr, (btn)))
#define MouseLeftClickedRaw()    WasMousePressedRaw(MouseBtn_Left)
#define MouseRightClickedRaw()   WasMousePressedRaw(MouseBtn_Right)
#define MouseMiddleClickedRaw()  WasMousePressedRaw(MouseBtn_Middle)
