#include "ch32v30x.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_usart.h"
#include "debug.h"

void SystemClock_Config_144MHz(void)
{
    RCC->CTLR |= RCC_HSEON;
    while (!(RCC->CTLR & RCC_HSERDY));

    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC | (15 << 18));  // PLLMUL = 18 → 8*18 = 144 MHz
    RCC->CTLR |= RCC_PLLON;
    while (!(RCC->CTLR & RCC_PLLRDY));

    RCC->CFGR0 &= ~RCC_SW;
    RCC->CFGR0 |= RCC_SW_PLL;
    while ((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL);

    RCC->CFGR0 |= (0 << 4);   // AHB  = 144 MHz
    RCC->CFGR0 |= (4 << 8);   // APB1 = 72 MHz
    RCC->CFGR0 |= (0 << 11);  // APB2 = 144 MHz

    FLASH->ACTLR = 0x02;      // 2 wait states
    SystemCoreClockUpdate();
}

// Инициализация UART1 (PA9 = TX)
void UART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = TX (Alternate function push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;        // только передача

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// Простая функция вывода строки
void UART_Print(const char* str)
{
    while (*str)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();

    SystemClock_Config_144MHz();
    UART1_Init();

    UART_Print("CH32V30x started!\r\n");
    UART_Print("Hello from wch.py monitor!\r\n\r\n");

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = GPIO_Pin_1,
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    uint32_t counter = 0;

    while(1)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
        Delay_Ms(500);

        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);
        Delay_Ms(500);

        // Выводим счётчик в монитор
        char buf[64];
        sprintf(buf, "Blink counter: %lu\r\n", counter++);
        UART_Print(buf);
    }
}

// #include "ch32v30x.h"
// #include "ch32v30x_gpio.h"
// #include "ch32v30x_misc.h"
// #include "debug.h"
// #include "system_ch32v30x.h"
//
// #define Sdds 90
//
// void SystemClock_Config_144MHz(void)
// {
//     RCC->CTLR |= (1 << 16);                    // HSEON
//     while (!(RCC->CTLR & (1 << 17)));
//
//     RCC->CFGR0 &= ~(0xF << 18);
//     RCC->CFGR0 |= (0x10 << 18);                // PLLMUL = 18 → 144 MHz
//     RCC->CFGR0 |= (1 << 16);                   // PLLSRC = HSE
//
//     RCC->CTLR |= (1 << 24);                    // PLLON
//     while (!(RCC->CTLR & (1 << 25)));
//
//     RCC->CFGR0 &= ~3;
//     RCC->CFGR0 |= 2;                           // SW = PLL
//
//     while ((RCC->CFGR0 & 0x0C) != 0x08);
//
//     RCC->CFGR0 |= (0 << 4);                    // AHB = 144 MHz
//     RCC->CFGR0 |= (4 << 8);                    // APB1 = 72 MHz
//     RCC->CFGR0 |= (0 << 11);                   // APB2 = 144 MHz
//
//     FLASH->ACTLR = 0x02;                       // Latency = 2
//
//     SystemCoreClockUpdate();
// }
//
// int main(void)
// {
//     // SystemClock_Config_144MHz();
//         NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//         Delay_Init();
//
//     RCC->CTLR |= (uint32_t)RCC_HSEON;                    // Включаем HSE
//     while((RCC->CTLR & RCC_HSERDY) == 0);                // Ждём стабилизации HSE
//
//     RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC | (0x0F << 10)); // PLLSRC = HSE, PLLMUL = 18 (8*18 = 144 МГц)
//     RCC->CTLR |= (uint32_t)RCC_PLLON;                    // Включаем PLL
//     while((RCC->CTLR & RCC_PLLRDY) == 0);                // Ждём стабилизации PLL
//
//     RCC->CFGR0 &= (uint32_t)~RCC_SW;                     // Переключаем SYSCLK на PLL
//     RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
//     while((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL);        // Ждём, пока SYSCLK переключится на PLL
//
//
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//     GPIO_InitTypeDef GPIO_InitStructure = {
//         .GPIO_Pin   = GPIO_Pin_1,
//         .GPIO_Mode  = GPIO_Mode_Out_PP,
//         .GPIO_Speed = GPIO_Speed_50MHz
//     };
//     GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//     // ================== ИЗМЕНИ ЭТО ЗНАЧЕНИЕ ==================
//     // delay = 0  → максимально возможная частота (~20-28 МГц)
//     // delay = 1  → ~12-15 МГц
//     // delay = 2  → ~8-9 МГц
//     // delay = 3  → ~6 МГц
//     // delay = 5  → ~4 МГц
//
//     while(1)
//     {
//             GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
//             Delay_Ms(1000);
//             GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);
//             Delay_Ms(1000);
//     }
// }
