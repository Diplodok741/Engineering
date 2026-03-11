// src/main.c
#include "ch32v20x.h"   // ← самый правильный заголовок для CH32V20x серии

int main(void)
{
    // Включаем тактирование GPIOB и AFIO (AFIO часто нужен)
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO;

    // PB0 как выход push-pull, 50 МГц
    GPIOB->CFGLR &= ~(GPIO_CFGLR_MODE0 | GPIO_CFGLR_CNF0);
    GPIOB->CFGLR |= GPIO_CFGLR_MODE0;           // 0b0011 = 50MHz output PP

    while (1)
    {
        GPIOB->BSHR = GPIO_BSHR_BS0;   // PB0 = 1 (set)
        for (volatile uint32_t i = 0; i < 800000; i++);  // задержка ~0.5–1 сек на 144 МГц

        GPIOB->BCR = GPIO_BCR_BR0;     // PB0 = 0 (reset)
        for (volatile uint32_t i = 0; i < 800000; i++);
    }
}
