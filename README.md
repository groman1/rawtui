# RawTUI by groman1
A lightweight TUI library for Unix written in C
# Documentation
init() : enable raw mode and save the terminal state\
deinit() : return to cooked mode\
setcursor(uint8_t status) : show/hide cursor (1/0)\
move(uint16_t y, uint16_t x) : move cursor to y,x coordinates\
clear() : clears the screen\
clearline() : clears the line the cursor is located on\
saveCursorPos() : saves cursor position\
loadCursorPos() : loads cursor position previously saved by saveCursorPos()\
getTermXY(uint16_t \*y, uint16_t \*x) : loads terminal width and height into x and y\
getCursorPos(uint16_t \*y, uint16_t \*x) : loads current cursor location into x and y\
wrattr(attr_t attr) : sets attribute to attr, use bitmasking with NORMAL, BLINK, REVERSE, BOLD, FAINT, macro COLORPAIR\
print(char \*string) : writes string to stdout\
initcolorpair(uint8_t id, uint8_t foreground, uint8_t background) : create a color pair with foreground and background specified with id from 1 to 7 (using 0 is not recommended as it is the default color pair)
COLORPAIR(pair) : use with wrattr as attribute\
moveprint(uint16_t y, uint16_t x, char \*string) : move to y,x and print string
# Notes and limitations
- The topmost location is 1,1; not 0,0 as it is in (n)curses
- RawTUI cant write to locations further than 999, 999
- For printing formatted text use dprintf(STDOUT_FILENO, **format**) or sprintf text into string and use print(char \*string)
- You can only create 8 color pairs
