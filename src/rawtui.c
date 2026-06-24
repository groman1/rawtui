#include <stdint.h>
#include <string.h>
#include "rawtui.h"

colorpair_t pairs[256];

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static struct termios originalterminal;

void initcolorpair(uint8_t id, uint8_t foreground, uint8_t background)
{
	pairs[id] = foreground<<4|background;
}

void init()
{
	tcgetattr(STDIN_FILENO, &originalterminal);
	struct termios terminal;
	cfmakeraw(&terminal);
	tcsetattr(STDIN_FILENO, 0, &terminal);
	initcolorpair(0, WHITE, BLACK);
	write(STDOUT_FILENO, "\x1b[?1049h", 8); // alternative buffer
}

void initinline()
{
	tcgetattr(STDIN_FILENO, &originalterminal);
	struct termios terminal;
	cfmakeraw(&terminal);
	tcsetattr(STDIN_FILENO, 0, &terminal);
	initcolorpair(0, WHITE, BLACK);
}

void deinit()
{
	tcsetattr(STDIN_FILENO, 0, &originalterminal);
	write(STDOUT_FILENO, "\x1b[?1049l", 8);
}


void setcursor(uint8_t status)
{
	if (status) write(STDOUT_FILENO, "\x1b[?25h", 6);
	else write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void move(uint16_t y, uint16_t x)
{
	++y;
	++x;
	char movecmd[10] = "\x1b[";
	int currPos = 2;
	for (int i = 100; i>0; i/=10)
	{
		if (i!=0&&y/i==0) continue;
		movecmd[currPos++] = y/i%10+48;
	}
	movecmd[currPos++] = ';';
	for (int i = 100; i>0; i/=10)
	{
		if (i!=1&&x/i==0) continue;
		movecmd[currPos++] = x/i%10+48;
	}
	movecmd[currPos++] = 'H';

	write(STDOUT_FILENO, movecmd, currPos);
}

uint8_t in(void)
{
	unsigned char ret;
	read(STDIN_FILENO, &ret, 1);
	return ret;
}

uint8_t inesc()
{
	unsigned char ret;
	read(STDIN_FILENO, &ret, 1);
	if (ret==27)
	{
		fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL)|O_NONBLOCK);
		unsigned char buffer[2];
		if (read(STDIN_FILENO, &buffer, 2)==-1) // ESC
		{
			fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL)^O_NONBLOCK);
			return ret;
		}
		switch (buffer[0])
		{
			case 'O':
			{
				switch (buffer[1])
				{
					case 'P': ret = 170; break; // F1
					case 'Q': ret = 171; break; // F2
					case 'R': ret = 172; break; // F3
					case 'S': ret = 173; break; // F4
				}
				break;
			}
			case '[':
			{
				switch (buffer[1])
				{
					case '1': case '2':
					{
						read(STDIN_FILENO, &buffer, 2);
						switch (buffer[0])
						{
							case '5': ret = 174; break; // F5
							case '7': ret = 175; break;	// F6
							case '8': ret = 176; break;	// F7
							case '9': ret = 177; break;	// F8
							case '0': ret = 178; break; // F9
							case '1': ret = 179; break; // F10
							case '3': ret = 180; break; // F11
							case '4': ret = 181; break; // F12
							case '~': ret = 182; break; // Insert
						}
						break;
					}
					case '3': ret = 183; read(STDIN_FILENO, buffer, 1); break; // Delete
					case 'H': ret = 184; break;	// Home
					case 'F': ret = 185; break; // End
					case '5': ret = 186; read(STDIN_FILENO, buffer, 1); break; // PageUp
					case '6': ret = 187; read(STDIN_FILENO, buffer, 1); break; // PageDn
					case 'A': ret = 188; break; // ArrowUp
					case 'B': ret = 189; break; // ArrowDown
					case 'C': ret = 190; break; // ArrowRight
					case 'D': ret = 191; break; // ArrowLeft
				}
				break;
			}
		}
		fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL)^O_NONBLOCK);
	}
	return ret;
}

void saveCursorPos()
{
	write(STDOUT_FILENO, "\x1b[s", 3);
}

void loadCursorPos()
{
	write(STDOUT_FILENO, "\x1b[u", 3);
}

void wrcolorpair(colorpair_t colorpair)
{
	char colorstring[8] = "\x1b[39;49m";
	if (colorpair)
	{
		colorstring[3] = (pairs[colorpair]>>4)+48;
		if (pairs[colorpair]%16) colorstring[6] = (pairs[colorpair]%16)+48;
	}
	write(STDOUT_FILENO, colorstring, 8);
}

void wrattr(attr_t attr)
{
	if (attr&BOLD) write(STDOUT_FILENO, "\x1b[1m", 4);
	if (attr&FAINT) write(STDOUT_FILENO, "\x1b[2m", 4);
	if (attr&REVERSE) write(STDOUT_FILENO, "\x1b[7m", 4);
	if (attr&UNDERLINE) write(STDOUT_FILENO, "\x1b[4m", 4);
	if (attr==NORMAL)
	{
		write(STDOUT_FILENO, "\x1b[0m", 4);
		wrcolorpair(0);
	}
}

void clear()
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
}

void cleartobot()
{
	write(STDOUT_FILENO, "\x1b[J", 3);
}

void cleartoeol()
{
	write(STDOUT_FILENO, "\x1b[K", 3);
}

void getTermXY(uint16_t *y, uint16_t *x)
{
	struct winsize win;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win)) if (ioctl(STDIN_FILENO, TIOCGWINSZ, &win)) if (ioctl(STDERR_FILENO, TIOCGWINSZ, &win)) return;
	*y = win.ws_row;
	*x = win.ws_col;
}

void getCursorPos(uint16_t *y, uint16_t *x)
{
	write(STDOUT_FILENO, "\x1b[6n", 4);
	*y = 0;
	*x = 0;
	char buffer[2];
	read(STDIN_FILENO, buffer, 2);
	for (int i = 0; i<3; ++i)
	{
		read(STDIN_FILENO, buffer, 1);
		if (*buffer==';') break;
		*y *= 10;
		*y += *buffer-48;
	}
	for (int i = 0; i<3; ++i)
	{
		read(STDIN_FILENO, buffer, 1);
		if (*buffer=='R') break;
		*x *= 10;
		*x += *buffer-48;
	}
	--*y;
	--*x;
}

void clearline()
{
	write(STDOUT_FILENO, "\x1b[2K", 4);
}

void printsize(char *string, int len)
{
	write(STDOUT_FILENO, string, len);
}

void moveprintsize(uint16_t y, uint16_t x, char *string, int len)
{
	move(y,x);
	printsize(string, len);
}

void print(char *string)
{
	printsize(string, strlen(string));
}

void moveprint(uint16_t y, uint16_t x, char *string)
{
	move(y,x);
	print(string);
}

void printc(char c)
{
	char s[2];
	s[0] = c;
	print(s);
}

#else // WINDOWS

#include <stdio.h>
#include <windows.h>
#include <conio.h>

void init()
{
	initcolorpair(0, WHITE, BLACK);
	clear();
	move(0,0);
}

static int lastPair = 0;

void initinline()
{
	initcolorpair(0, WHITE, BLACK);
	// the move(0,0) is not intended to be used here, however i have noticed that the getTermXY function may not work properly. not included for now
}

void deinit()
{
	setcursor(1);
	wrcolorpair(0);
	clear();
}

void setcursor(uint8_t visible)
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void move(uint16_t y, uint16_t x)
{
	COORD location = {(short)x,(short)y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), location);
}

void initcolorpair(uint8_t id, uint8_t foreground, uint8_t background)
{
	if (foreground>WHITE || background>WHITE) return;
	pairs[id] = (foreground << 4) | background;
}

uint8_t in()
{
	return _getch();
}

uint8_t inesc()
{
	uint8_t input = _getch();
	if (input == 0)
	{
		input = _getch();
		if (input >= 59 && input <= 68) return 170 + (input - 59);
	}
	else if (input == 224)
	{
		switch (_getch())
		{
			case 82: return 182;
			case 83: return 183;
			case 71: return 184;
			case 79: return 185;
			case 73: return 186;
			case 81: return 187;
			case 72: return 188;
			case 80: return 189;
			case 77: return 190;
			case 75: return 191;
		}
	}
	return input;
}

// should work on windows 10/11, windows 7 doesn't support ansi escape sequences ( untested )

void clear()
{
	uint16_t x, y;
	getTermXY(&y, &x);
	for (int i = 0; i<y; ++i)
		for (int t = 0; t<x; ++t)
			printc(' ');
}

void cleartobot()
{
	fputs("\x1b[J", stdout);
}

void cleartoeol()
{
	fputs("\x1b[K", stdout);
}

static uint16_t savedX = 0, savedY = 0;
void saveCursorPos()
{
	getCursorPos(&savedY, &savedX);
}

void loadCursorPos()
{
	move(savedY, savedX);
}

void wrattr(attr_t attr)
{
	if (attr&BOLD) fputs("\x1b[1m", stdout);
	if (attr&FAINT) fputs("\x1b[2m", stdout);
	if (attr&REVERSE) fputs("\x1b[7m", stdout);
	if (attr&UNDERLINE) fputs("\x1b[4m", stdout);
	if (attr == NORMAL)
	{
		fputs("\x1b[0m", stdout);
		wrcolorpair(0);
	}
}

void wrcolorpair(colorpair_t colorpair)
{
	lastPair = colorpair;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), pairs[colorpair]);
}

void getTermXY(uint16_t* y, uint16_t* x)
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);
	*y = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
	*x = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
}

void getCursorPos(uint16_t* y, uint16_t* x)
{
	POINT point;
	GetCursorPos(&point);
	*y = point.y;
	*x = point.x;
}

void clearline()
{
	fputs("\x1b[2K", stdout);
}

void print(char* string)
{
	fputs(string, stdout);
}

void printc(char c)
{
	putchar(c);
}

void printsize(char* string, int len)
{
	for (int i = 0; i < len; ++i)
		putchar(string[i]);
}

void moveprint(uint16_t y, uint16_t x, char* string)
{
	move(y, x);
	print(string);
}

void moveprintsize(uint16_t y, uint16_t x, char* string, int len)
{
	move(y, x);
	printsize(string, len);
}

#endif
