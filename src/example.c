#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include "rawtui.h"

#ifdef _WIN32
#define dprintf(output, ...) printf(__VA_ARGS__)
#endif

// Example program: prints every key pressed with underline and terminal rows/columns

int main()
{
	uint16_t y, x;
	uint16_t code = 13;
	char *string;
	init();
	getTermXY(&y, &x);
	clear();
	setcursor(0);
	move(y/2-2,x/2-3);
	dprintf(STDOUT_FILENO, "%d:%d", y, x);
	initcolorpair(1, BLACK, RED);
	wrcolorpair(1);
	do
	{
		move(y/2, x/2-1);
		wrcolorpair(0);
		wrattr(NORMAL);
		clearline(); // if you use color and call clearline, the line will be filled with that color, so we need to disable it first
		wrcolorpair(1);
		wrattr(UNDERLINE);
		dprintf(STDOUT_FILENO, "%u", code);
	}
	while((code=inesc())!=3);
	wrcolorpair(0);
	wrattr(NORMAL);
	setcursor(1);
	clear();
	deinit();
	return 0;
}
