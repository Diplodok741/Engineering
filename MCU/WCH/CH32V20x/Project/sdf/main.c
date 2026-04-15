#include "ch32v20x_gpio.h"
#include "debug.h"
#include <ch32v20x.h>

int main(){
        Delay_Init();
        GPIO_InitTypeDef gpio = {
                .GPIO_Mode = GPIO_Mode_Out_PP,
                .GPIO_Pin = 15,
                .GPIO_Speed = GPIO_Speed_10MHz
        };
        GPIO_Init(GPIOB, &gpio);

        while (1) {
                GPIO_SetBits(GPIOB, 15);
                Delay_Ms(500);
                GPIO_ResetBits(GPIOB, 15);
                Delay_Ms(500);
        }
        return 0;
}
