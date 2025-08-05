#include <stdint.h>
#include <stdio.h>
#include "rawtui.h"

// Example program: prints every key pressed and terminal rows/columns

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
	wrattr(COLORPAIR(1));
	do
	{
		move(y/2, x/2-1);
		wrattr(NORMAL);
		clearline(); // if you use color and call clearline, the line will be filled with that color, so we need to disable it first
		wrattr(COLORPAIR(1));
		dprintf(STDOUT_FILENO, "%u", code);
	}
	while((code=in())!=3);
	wrattr(NORMAL);
	clear();
	setcursor(1);
	move(0,0);
	deinit();
	return 0;
}
