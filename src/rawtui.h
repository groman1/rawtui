#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#ifndef RAWTUI_H_
#define RAWTUI_H_

#define attr_t uint8_t
#define NORMAL 1
#define BOLD 1<<1
#define FAINT 1<<2
#define REVERSE 1<<3

void init();
void deinit();
void setcursor(uint8_t visible);
void move(uint8_t y, uint8_t x);
uint16_t in();
void clear();
void saveCursorPos();
void loadCursorPos();
void wrattr(attr_t attr);
void getTermXY(uint8_t *y, uint8_t *x);
void getCursorPos(uint8_t *y, uint8_t *x);
void clearline();
void print(char *string);

#endif
