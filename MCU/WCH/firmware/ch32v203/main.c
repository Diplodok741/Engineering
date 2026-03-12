#include "ch32v20x.h"

void delay(uint32_t count) {
    while(count--) __asm__ volatile ("nop");
}

int main(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
    GPIOC->CFGLR &= ~(0xF << (13 * 4));
    GPIOC->CFGLR |= 0x1 << (13 * 4); // push-pull output

    while(1) {
        GPIOC->BSHR = 1 << 13;   // LED on
        delay(100000);
        GPIOC->BCR = 1 << 13;    // LED off
        delay(100000);
    }
}
