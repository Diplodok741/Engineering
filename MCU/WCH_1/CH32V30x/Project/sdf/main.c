#include "ch32v30x_gpio.h"
#include <ch32v30x_eth.h>
#include <ch32v30x.h>

int main(){
        GPIO_InitTypeDef gpio = {
                .GPIO_Mode = GPIO_Mode_Out_PP,
                .GPIO_Pin = 15,
                .GPIO_Speed = GPIO_Speed_10MHz
        };
        GPIO_Init(GPIOB, &gpio);
        return 0;
}
