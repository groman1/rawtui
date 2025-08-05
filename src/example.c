#include <stdint.h>
#include <stdio.h>
#include "rawtui.h"

int main()
{
	uint8_t y, x;
	uint16_t code;
	char *string;
	init();
	getTermXY(&y, &x);
	clear();
	setcursor(0);
	move(8,100);
	dprintf(STDOUT_FILENO, "%d:%d", y, x);
	while((code=in())!=3)
	{
		move(10, 101);
		clearline();
		dprintf(STDOUT_FILENO, "%u", code);
	}
	clear();
	setcursor(1);
	move(1,1);
	deinit();
	return 0;
}
