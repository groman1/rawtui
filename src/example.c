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
	wrattr(REVERSE);
	do
	{
		move(10, 101);
		clearline();
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
