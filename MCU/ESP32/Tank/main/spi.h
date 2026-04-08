#include <freertos/FreeRTOS.h>
#include "driver/spi_common.h"
#include "hal/spi_types.h"
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

#define mosiPin 11
#define misoPin 12
#define sclkPin 13
#define csPin   10

void SpiInit();
void SpiTransaction();
