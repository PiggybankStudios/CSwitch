# C-Switch
A simple project to turn on/off defines in a C/C++ header file quickly. This project is based on [PigCore](https://github.com/PiggybankStudios/PigCore), it uses [sokol_gfx.h](https://github.com/floooh/sokol/blob/master/sokol_gfx.h) and [sokol_app.h](https://github.com/floooh/sokol/blob/master/sokol_app.h) to make a graphical window, and [Clay](https://github.com/nicbarker/clay) to do UI layout.

# Usage
1. Run **cswitch.exe**
2. Select File->Open from the top bar (or drag and drop a file from Explorer)
3. Choose a C/C++ header file (.h/.hpp) that has some number of `#define` lines that are set to values like `1`, `0`, `true`, or `false` (value-less `#define`s will also work, they will be commented/uncommented)
4. After opening, a button for each `#define` in the file will be shown.
5. Clicking a button will toggle the value between 1 and 0, or true and false (and immediately write to the file)

# Screenshot
![Screenshot 1](/_media/screenshot8.png)

![Screenshot 2](/_media/screenshot7.png)

![Screenshot 3](/_media/screenshot6.png)

# Hotkeys
- **Ctrl+W** = Close Current Tab
- **Ctrl+Shift+W** = Close Window
- **Ctrl+O** = Open File
- **Ctrl+E** = Open Most Recent File
- **Ctrl+T** = Toggle Topmost
- **Ctrl+Tab** = Switch to Next Tab
- **Ctrl+Shift+Tab** = Switch to Previous Tab
- **Middle Click (On Tab)** = Close Tab
- **Ctrl+Plus/Minus or Ctrl+Scroll** - Increase/Decrease the Font Size / UI scale
- **F10** - Toggle Small Button Mode
- **F11** = Toggle Topbar (also can use Escape to bring the topbar back when it's hidden)
- **Alt+F** = Open File Menu (useful when Topbar is hidden)
- **Alt+V** = Open View Menu (useful when Topbar is hidden)
- **Home/End** - Scroll to Beginning/End
- **PageUp/PageDown** - Scroll Up/Down One Screen Height at a Time

# Command-Line Arguments
- Any unnamed argument will be treated as a file path to open on startup, if no path is given, the most recently opened file is opened automatically (if it still exists)
- `--top`/`--topmost` = Enable topmost (Windows Only)
- `-s=w,h`/`--size=w,h` = Change the size of the window
