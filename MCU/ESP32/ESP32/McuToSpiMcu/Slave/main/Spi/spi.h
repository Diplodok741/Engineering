#ifndef SPI_H

#include "driver/spi_common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <freertos/queue.h>
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "hal/spi_types.h"
#include "xtensa/hal.h"
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <driver/spi_slave.h>
#include <driver/gpio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "esp_attr.h"


#define slaveMosiPin 23
#define slaveMisoPin 19
#define slaveChipSelectPin 5
#define slaveSclkPin 18 

struct stringData {
        char unit[8];
        char value[8];
        char degree[8];
}__attribute__((packed));

void SpiInit();
struct stringData SpiTransaction();


#endif // !SPI_H

