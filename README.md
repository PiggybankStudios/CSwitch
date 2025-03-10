# C-Switch
A simple project to turn on/off defines in a C/C++ header file quickly. This project is based on [PigCore](https://github.com/PiggybankStudios/PigCore), it uses [sokol_gfx.h](https://github.com/floooh/sokol/blob/master/sokol_gfx.h) and [sokol_app.h](https://github.com/floooh/sokol/blob/master/sokol_app.h) to make a graphical window, and [Clay](https://github.com/nicbarker/clay) to do UI layout.

# Usage
1. Run **cswitch.exe**
2. Select File->Open from the top bar (or drag and drop a file from Explorer)
3. Choose a C/C++ header file (.h/.hpp) that has some number of `#define` lines that are set to values like `1`, `0`, `true`, or `false` (value-less `#define`s will also work, they will be commented/uncommented)
4. After opening, a button for each `#define` in the file will be shown.
5. Clicking a button will toggle the value between 1 and 0, or true and false (and immediately write to the file)

# Screenshot
![Screenshot4](/_media/screenshot4.png)
![Screenshot5](/_media/screenshot5.png)

# Hotkeys
- **Ctrl+W** = Close file
- **Ctrl+Shift+W** = Close Window
- **Ctrl+O** = Open File
- **Ctrl+E** = Open Most Recent File
- **Ctrl+T** = Toggle Topmost

# Command-Line Arguments
- Any unnamed argument will be treated as a file path to open on startup, if no path is given, the most recently opened file is opened automatically (if it still exists)
- `--top`/`--topmost` = Enable topmost (Windows Only)
- `-s=w,h`/`--size=w,h` = Change the size of the window
