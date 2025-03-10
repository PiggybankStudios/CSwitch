# C-Switch
A simple project to turn on/off defines in a C header file quickly. This project is based on PigCore using sokol_gfx.h and sokol_app.h to make a graphical window.

# Usage
1. Run cswitch.exe
2. Select File->Open from the top bar
3. Choose a C header file (.h) that has some number of #define lines that are set to values like `1`, `0`, `true`, or `false` (value-less `#define`s will also work, they will be commented/uncommented)
4. After opening, a button for each `#define` in the file will be shown.
5. Clicking a button will toggle the value between 1 and 0, or true and false (and immediately write to the file)

# Screenshot
![Screenshot4](/_media/screenshot4.png)
![Screenshot5](/_media/screenshot5.png)
