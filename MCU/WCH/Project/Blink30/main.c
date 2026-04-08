#include "ch32v30x.h"        // Для CH32V30x
#include "ch32v30x_gpio.h"


int main(void)
{
    // Включаем тактирование порта C
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Pin = GPIO_Pin_15,
            .GPIO_Mode = GPIO_Mode_Out_PP,
            .GPIO_Speed = GPIO_Speed_2MHz,
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    while(1)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_15);     // LED выкл (если анод к VCC)
        GPIO_ResetBits(GPIOC, GPIO_Pin_15);   // LED вкл
    }
}
