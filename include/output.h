#ifndef OUTPUT_H
#define OUTPUT_H

#include <time.h>
#include "types.h"

void printf_RGB(int r, int g, int b, const char* format, ...);
void printf_RGB_BG(int fr, int fg, int fb, int br, int bg, int bb, const char* format, ...);
void printf_wave_utf8(int r1, int g1, int b1, int r2, int g2, int b2, const char* text, int step, int* out_r, int* out_g, int* out_b);
void sizeToGradientColor(long long size, int* r, int* g, int* b);
void printf_Stat();
void printf_debug(Param p,clock_t start,clock_t end);
void printf_searched();
int* print_rainbow(int* rgb, float brightness, const char* string, int pas, int sens, int max);
void printBarre(int _Size, int value);
void printf_error(int error);

#endif