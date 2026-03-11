#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata, _sdata, _edata;
extern uint32_t _sbss, _ebss;

void Reset_Handler(void);
void Default_Handler(void) { while (1); }

// Слабые обработчики (переопредели нужные потом)
void NMI_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler (void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler   (void) __attribute__((weak, alias("Default_Handler")));
// ... добавь UART, TIM, EXTI и т.д. по мере необходимости

// Векторная таблица
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void)) &_estack,          // 0  Initial Stack Pointer
    Reset_Handler,                      // 1  Reset
    NMI_Handler,                        // 2
    HardFault_Handler,                  // 3
    0,                                  // 4  Reserved
    0,                                  // 5  Reserved
    0,                                  // 6  Reserved
    0,                                  // 7  Reserved
    0,                                  // 8  Reserved
    0,                                  // 9  Reserved
    0,                                  // 10 Reserved
    SysTick_Handler,                    // 11 SysTick (пример)
    // Дальше идут EXTI, TIM, UART, ADC и т.д. — заполни нулями или Default_Handler
    // Всего в CH32V203 обычно ~60–80 векторов, но для старта хватит первых 12
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // ... можно добавить до конца, если нужно
};

void Reset_Handler(void)
{
    uint32_t *src, *dst;

    // Копируем .data из flash в RAM
    src = &_sidata;
    dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    // Зануляем .bss
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    // Переход в main
    main();

    while (1);  // на всякий случай
}
