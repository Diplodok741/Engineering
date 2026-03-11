#ifndef SPI_H
#define SPI_H

#include "driver/spi_common.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_memory_utils.h"
#include "hal/spi_types.h"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define logInfo ESP_LOGI
#define logWarn ESP_LOGW
#define logError ESP_LOGE
#define spiChipSelectPin 5
#define spiMosiPin 23
#define spiMisoPin 19
#define spiSclkPin 18

extern spi_device_handle_t spiControl;

void SpiInit();
void SpiTransaction(uint8_t value, uint8_t degree, char* unit);

#endif // !SPI_H
