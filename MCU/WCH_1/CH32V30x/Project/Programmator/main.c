#include <stdint.h>
#include <stdio.h>
#include "ch32v30x_rcc.h"
#include "debug.h"
#include "ssd1306/i2c_tx.h"
#include "system_ch32v30x.h"
#include <ch32v30x.h>
#include "ssd1306/oled_segment.h"

int main(){
        USART_Printf_Init(115200);
        SystemCoreClockUpdate();
        SystemInit();
        Delay_Init();

        printf("Инициализация I2C");
        I2C_init();
        printf("Инициализация Oled Дисплея");
        OLED_init();
        OLED_setpos(10, 10);
        OLED_clear();

        while (1) {
                OLED_print("Hello");
                Delay_Ms(1000);
        }
        return 0;
}
