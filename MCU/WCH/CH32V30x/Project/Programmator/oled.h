#include <ch32v30x_i2c.h>
#include <stdio.h>
#include "ssd1306/oled_segment.h"

void Menu();
void StartScreen();
void FillRect(int x1, int y1, int x2, int y2, bool white);
void DrawLine(int x1, int y1, int x2, int y2, bool white);
