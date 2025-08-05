#include <stdint.h>
#include <stdio.h>
#include "rawtui.h"

// Example program: prints every key pressed and terminal rows/columns

int main()
{
	uint8_t y, x;
	uint16_t code = 13;
	char *string;
	init();
	getTermXY(&y, &x);
	clear();
	setcursor(0);
	move(8,100);
	dprintf(STDOUT_FILENO, "%d:%d", y, x);
	initcolorpair(1, BLACK, RED);
	wrattr(COLORPAIR(1));
	do
	{
		move(10, 101);
		wrattr(NORMAL);
		clearline(); // if you use color and call clearline, the line will be filled with that color, so we need to disable it first
		wrattr(COLORPAIR(1));
		dprintf(STDOUT_FILENO, "%u", code);
	}
	while((code=in())!=3);
	wrattr(NORMAL);
	clear();
	setcursor(1);
	move(1,1);
	deinit();
	return 0;
}
