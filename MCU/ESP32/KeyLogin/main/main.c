#include <stdint.h>
#include <stdio.h>
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "nfc.h"

void app_main(void)
{
        SpiInit();
        while (1) {
                // uint16_t data = SpiTransaction();
                SpiTransaction();
                vTaskDelay(pdMS_TO_TICKS(2000));
        }
}
