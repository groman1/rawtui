# RawTUI by groman1
A lightweight TUI library for Unix written in C
# Documentation
init() : enable raw mode and save the terminal state\
deinit() : return to cooked mode\
setcursor(uint8_t status) : show/hide cursor (1/0)\
move(uint8_t y, uint8_t x) : move cursor to y,x coordinates\
clear() : clears the screen\
clearline() : clears the line the cursor is located on\
saveCursorPos() : saves cursor position\
loadCursorPos() : loads cursor position previously saved by saveCursorPos()\
getTermXY(uint8_t \*y, uint8_t \*x) : loads terminal width and height into x and y\
getCursorPos(uint8_t \*y, uint8_t \*x) : loads current cursor location into x and y\
wrattr(attr_t attr) : sets attribute to attr, use bitmasking with NORMAL, BLINK, REVERSE, BOLD, FAINT, etc, defined in rawtui.h\
print(char \*string) : writes string to stdout
# Notes
- The topmost location is 1,1; not 0,0 as it is in (n)curses
- RawTUI cant write to locations further than 255, 255 because the coordinates are 8 bit unsigned integers
- For printing formatted text use dprintf(STDOUT_FILENO, **format**) or sprintf text into string and use print(char \*string)
