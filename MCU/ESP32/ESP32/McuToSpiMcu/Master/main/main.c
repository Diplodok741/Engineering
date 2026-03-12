#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include "Spi/Spi.h"

#define logInfo ESP_LOGI


void app_main(void)
{
        SpiInit();
        while (1) {
                SpiTransaction(50, 2, "V");
                vTaskDelay(6000/portTICK_PERIOD_MS);
        }
}
