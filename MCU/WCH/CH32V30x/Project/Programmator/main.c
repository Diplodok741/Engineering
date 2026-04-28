#include <stdint.h>
#include <stdio.h>
#include "ch32v30x_rcc.h"
#include "debug.h"
#include "ssd1306/i2c_tx.h"
#include "system_ch32v30x.h"
#include <ch32v30x.h>
#include "ssd1306/oled_segment.h"
#include "oled.h"

inline void ServiceInit();

int main(){
        // ServiceInit();
        Delay_Init();
        SystemInit();
        USART_Printf_Init(115200);
        SystemCoreClockUpdate();
        printf("Инициализация I2C");
        I2C_init();
        printf("Инициализация Oled Дисплея");
        OLED_init();

        // FillRect(10, 9, 32, 10, 1);
        DrawLine(10, 2, 20, 4, 1);

        return 0;
}

inline void ServiceInit(){
        Delay_Init();
        SystemInit();
        USART_Printf_Init(115200);
        SystemCoreClockUpdate();
        printf("Инициализация I2C");
        I2C_init();
        printf("Инициализация Oled Дисплея");
        OLED_init();
}
