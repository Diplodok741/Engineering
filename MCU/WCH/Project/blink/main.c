#include "ch32v20x.h"        // Для CH32V30x

// void Delay_Ms(uint32_t ms)
// {
//     uint32_t i;
//     for(i = 0; i < (ms * 8000); i++)   // примерно для 8 МГц после сброса
//     {
//         __asm volatile("nop");
//     }
// }

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
