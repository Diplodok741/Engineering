#ifndef SPI_H

#include "driver/spi_common.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "hal/spi_types.h"
#include "xtensa/hal.h"
#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <driver/spi_slave.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>


#define slaveMosiPin 23
#define slaveMisoPin 19
#define slaveChipSelectPin 5
#define slaveSclkPin 18 


void SpiInit();
uint8_t SpiTransaction();


#endif // !SPI_H
