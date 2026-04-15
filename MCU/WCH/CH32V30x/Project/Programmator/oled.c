#include "oled.h"

void Menu(){
        OLED_setpos(0, 0);
        OLED_clear();

        while (1) {
                OLED_print("Hello");
                Delay_Ms(1000);
        }


}
