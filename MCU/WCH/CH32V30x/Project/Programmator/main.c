#include <stdint.h>
#include <stdio.h>
#include "ch32v30x_rcc.h"
#include "debug.h"
#include "ssd1306/i2c_tx.h"
#include "system_ch32v30x.h"
#include <ch32v30x.h>
#include "ssd1306/oled_segment.h"

int main(){
        Delay_Init();
        SystemInit();
        USART_Printf_Init(115200);
        SystemCoreClockUpdate();

        I2C_init();
        OLED_init();
        OLED_setpos(0, 0);
        while (1) {
                OLED_print("Hello");
                Delay_Ms(500);
        }

        return 0;
}
