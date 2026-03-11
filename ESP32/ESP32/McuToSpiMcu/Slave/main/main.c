#include <stdio.h>
#include "Display/display.h"
#include "Spi/spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

QueueHandle_t dataTask;

void app_main(void)
{
        SpiInit();
        DisplayInit();
        while (1) {
                // SpiTransaction();
                Scale();
                ButtonStruct();
                vTaskDelay(6000/portTICK_PERIOD_MS);
        }
}
