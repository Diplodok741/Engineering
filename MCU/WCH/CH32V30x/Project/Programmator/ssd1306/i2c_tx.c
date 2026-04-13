#include "i2c_tx.h"
#include "ch32v30x.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_i2c.h"

#ifndef F_CPU
  #define F_CPU           24000000  // 24Mhz if not otherwise defined
#endif

#define I2C_START_GENERATED     0x00010003    // BUSY, MSL, SB
#define I2C_ADDR_TRANSMITTED    0x00820003    // BUSY, MSL, ADDR, TXE
#define I2C_BYTE_TRANSMITTED    0x00840003    // BUSY, MSL, BTF, TXE
#define I2C_checkEvent(n)       (((((uint32_t)I2C1->STAR1<<16) | I2C1->STAR2) & n) == n)

void I2C_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);

    I2C_InitTypeDef i2c = {
            .I2C_ClockSpeed = 100000,
            .I2C_Mode = I2C_Mode_I2C,
            .I2C_DutyCycle = I2C_DutyCycle_16_9,
            .I2C_OwnAddress1 = 0x74,
            .I2C_Ack = I2C_Ack_Enable,
            .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
    };
    I2C_Init(I2C1, &i2c);   
    I2C_Cmd(I2C1, ENABLE);

    GPIO_InitTypeDef gpio = {
            .GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9,
            .GPIO_Mode = GPIO_Mode_AF_OD,
            .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOB, &gpio);
  // #if I2C_MAP == 0
  // RCC->APB2PCENR |= RCC_IOPBEN | RCC_AFIOEN;
  // RCC->APB1PCENR |= RCC_I2C1EN;
  //
  // GPIO_InitTypeDef gpioI2cInit = {0};
  //
  // gpioI2cInit.GPIO_Mode = GPIO_Mode_AF_OD;
  // gpioI2cInit.GPIO_Pin = GPIO_Pin_6;
  // gpioI2cInit.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOB, &gpioI2cInit);
  //
  // gpioI2cInit.GPIO_Mode = GPIO_Mode_AF_OD;
  // gpioI2cInit.GPIO_Pin = GPIO_Pin_7;
  // gpioI2cInit.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOB, &gpioI2cInit);
  //
  // #elif I2C_MAP == 1
  // // Remap I2C pins, enable GPIO port D and I2C module
  // RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN;
  // RCC->APB1PCENR |= RCC_I2C1EN;
  // AFIO->PCFR1    |= 1<<1;
  //
  // // Set pin PD0 (SDA) and PD1 (SCL) to output, open-drain, 10MHz, multiplex
  // GPIOD->CFGLR = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
  //                              |  (((uint32_t)0b1101<<(0<<2)) | ((uint32_t)0b1101<<(1<<2)));
  // #elif I2C_MAP == 2
  // // Remap I2C pins, enable GPIO port C and I2C module
  // RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  // RCC->APB1PCENR |= RCC_I2C1EN;
  // AFIO->PCFR1    |= 1<<22;
  //
  // // Set pin PC6 (SDA) and PC5 (SCL) to output, open-drain, 10MHz, multiplex
  // GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(5<<2))))
  //                              |  (((uint32_t)0b1101<<(6<<2)) | ((uint32_t)0b1101<<(5<<2)));
  // #else
  //   #warning Wrong I2C REMAP
  // #endif
  //
  // // Set I2C module clock frequency (in MHz)
  // I2C1->CTLR2 = 4;
  //
  // // Set bus clock configuration
  // I2C1->CKCFGR = (F_CPU / (3 * I2C_CLKRATE)) | I2C_CKCFGR_FS;
  //
  // I2C1->CTLR1 |= (1<<1);
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  while(I2C1->STAR2 & I2C_STAR2_BUSY);            // wait until bus ready
  I2C1->CTLR1 |= I2C_CTLR1_START;                 // set START condition
  while(!(I2C1->STAR1 & I2C_STAR1_SB));           // wait for START generated
  I2C1->DATAR = addr;                             // send slave address + R/W bit
  while(!I2C_checkEvent(I2C_ADDR_TRANSMITTED));   // wait for address transmitted
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2C1->STAR1 & I2C_STAR1_TXE));          // wait for last byte transmitted
  I2C1->DATAR = data;                             // send data byte
}

// Stop I2C transmission
void I2C_stop(void) {
  while(!(I2C1->STAR1 & I2C_STAR1_BTF));          // wait for last byte transmitted
  I2C1->CTLR1 |= I2C_CTLR1_STOP;                  // set STOP condition
}
