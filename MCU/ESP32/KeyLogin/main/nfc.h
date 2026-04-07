#include "hal/spi_types.h"
#include <driver/spi_master.h>
#include <driver/spi_common.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <stdint.h>

#define chipSelect 5
#define mosi 23
#define miso 19
#define sclk 18
#define reset 22

void SpiInit();
void SpiTransaction();
