#include "ch32v20x.h"        // Для CH32V30x

int main(void)
{
    // Включаем тактирование порта C
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Настраиваем PC15 как выход push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    while(1)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_15);     // LED выкл (если анод к VCC)
        Delay_Ms(500);

        GPIO_ResetBits(GPIOC, GPIO_Pin_15);   // LED вкл
        Delay_Ms(500);
    }
}
