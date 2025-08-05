#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>

#define attr_t uint8_t
#define NORMAL 1
#define BOLD 1<<1
#define FAINT 1<<2
#define REVERSE 1<<3

struct termios originalterminal;

void init()
{
	tcgetattr(STDIN_FILENO, &originalterminal);
	struct termios terminal;
	cfmakeraw(&terminal);
	tcsetattr(STDIN_FILENO, 0, &terminal);
}

void deinit()
{
	tcsetattr(STDIN_FILENO, 0, &originalterminal);
}


void setcursor(uint8_t status)
{
	if (status) write(STDOUT_FILENO, "\x1b[?25h", 6);
	else write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void move(uint8_t y, uint8_t x)
{
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

uint16_t in(void)
{
	char ret;
	read(STDIN_FILENO, &ret, 1);
	return ret;
}

void clear()
{
	write(STDOUT_FILENO, "\x1b[1J", 4);
}

void saveCursorPos()
{
	write(STDOUT_FILENO, "\x1b[s", 3);
}

void loadCursorPos()
{
	write(STDOUT_FILENO, "\x1b[u", 3);
}

void wrattr(attr_t attr)
{
	if (attr&NORMAL) write(STDOUT_FILENO, "\x1b[0m", 4);
	if (attr&BOLD) write(STDOUT_FILENO, "\x1b[1m", 4);
	if (attr&FAINT) write(STDOUT_FILENO, "\x1b[2m", 4);
	if (attr&REVERSE) write(STDOUT_FILENO, "\x1b[7m", 4);
}

void getTermXY(uint8_t *y, uint8_t *x)
{
	struct winsize win;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win)) if (ioctl(STDIN_FILENO, TIOCGWINSZ, &win)) if (ioctl(STDERR_FILENO, TIOCGWINSZ, &win)) return;
	*y = win.ws_row;
	*x = win.ws_col;
}

void getCursorPos(uint8_t *y, uint8_t *x)
{
	write(STDOUT_FILENO, "\x1b[6n", 4);
	*y = 0;
	*x = 0;
	char buffer[2];
	int currPos = 2;
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
}

void clearline()
{
	write(STDIN_FILENO, "\x1b[2K", 4);
}

void print(char *string)
{
	write(STDOUT_FILENO, string, strlen(string));
}
