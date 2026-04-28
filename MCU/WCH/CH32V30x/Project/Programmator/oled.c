#include "oled.h"
#include "debug.h"
#include "ssd1306/oled_segment.h"

void StartScreen(){
        char k = 26, s = 100;
        
        for (char j=1; j<6; j+=2) {
                OLED_DrawPixel(27, 0, 1);
                OLED_DrawPixel(k, j, 1);
                OLED_DrawPixel(k, j+1, 1);
                OLED_DrawPixel(99, 0, 1);
                OLED_DrawPixel(s, j, 1);
                OLED_DrawPixel(s, j+1, 1);
                k--;
                s++;
                OLED_DisplayBuffer();
        }

        for (char j=7; j<23; j+=2) {
                OLED_DrawPixel(24, j, 1);
                OLED_DrawPixel(24, j+1, 1);
                OLED_DrawPixel(102, j, 1);
                OLED_DrawPixel(102, j+1, 1);
                OLED_DisplayBuffer();
        }
        k = 24; 
        s = 102;

        for (char j=23; j<30; j+=2) {
                OLED_DrawPixel(k, j, 1);
                OLED_DrawPixel(k, j+1, 1);
                OLED_DrawPixel(s, j, 1);
                OLED_DrawPixel(s, j+1, 1);
                k++;
                s--;
                OLED_DisplayBuffer();
        }

        char i=31;
        while (i<96) {
                OLED_DrawLine(1, i, 1);
                OLED_DisplayBuffer();
                i+=2;
        }

        OLED_clear();

        char a = 26, b = 100;

        for (char j=1; j<6; j+=2) {
                OLED_DrawPixel(27, 0, 1);
                OLED_DrawPixel(a, j, 1);
                OLED_DrawPixel(a, j+1, 1);
                OLED_DrawPixel(99, 0, 1);
                OLED_DrawPixel(b, j, 1);
                OLED_DrawPixel(b, j+1, 1);
                a--;
                b++;
                OLED_DisplayBuffer();
        }

        for (char j=7; j<23; j+=2) {
                OLED_DrawPixel(24, j, 1);
                OLED_DrawPixel(24, j+1, 1);
                OLED_DrawPixel(102, j, 1);
                OLED_DrawPixel(102, j+1, 1);
                OLED_DisplayBuffer();
        }

        a = 24;
        b = 102;

        for (char j=23; j<30; j+=2) {
                OLED_DrawPixel(a, j, 1);
                OLED_DrawPixel(a, j+1, 1);
                OLED_DrawPixel(b, j, 1);
                OLED_DrawPixel(b, j+1, 1);
                a++;
                b--;
                OLED_DisplayBuffer();
        }

        OLED_setpos(37, 1);
        OLED_print("Completed");
        OLED_setpos(34, 2);
        OLED_print("Successfull");
        Delay_Ms(500);
        // OLED_clear();
}

void DrawLine(int x1, int y1, int x2, int y2, bool white){
        if (x2 < x1 || y2 < y1) {
                return;
        }

        for (int i=x1; i<x2; i++) {
                        OLED_DrawPixel(i, y1, white);
                for (int j=y1; j<y2; j++) {
                        OLED_DrawPixel(x1, j, white);
                }
        }
        OLED_DisplayBuffer();
}

void FillRect(int x1, int y1, int x2, int y2, bool white){
        if (x2 < x1 || y2 < y1) {
                return;
        }

        for (int i=x1; i<x2; i++) {
                for (int j=y1; j<y2; j++) {
                        OLED_DrawPixel(i, j, white);
                }
        }
        OLED_DisplayBuffer();
}

void Menu(){
        OLED_setpos(0, 0);
        OLED_clear();

        while (1) {
                OLED_print("Hello");
                Delay_Ms(1000);
        }


}
