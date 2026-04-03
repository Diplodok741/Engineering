#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/idf_additions.h>
#include "driver/gpio.h"
#include "freertos/projdefs.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"
#include <driver/ledc.h>
#include <stdint.h>

void app_main(void)
{
        gpio_set_direction(13, GPIO_MODE_OUTPUT);
        // Настройка таймеров
        ledc_timer_config_t timerConfig = {
                .clk_cfg = LEDC_AUTO_CLK,
                .speed_mode = LEDC_HIGH_SPEED_MODE,
                .duty_resolution = LEDC_TIMER_10_BIT,
                .freq_hz = 5000,
                .timer_num = LEDC_TIMER_0,
        };
        ledc_timer_config(&timerConfig);

        ledc_channel_config_t ledcConfig = {
                .duty = 0,
                .hpoint = 0,
                .gpio_num = 13,
                .speed_mode = LEDC_HIGH_SPEED_MODE,
                .channel = LEDC_CHANNEL_0, 
                .intr_type = LEDC_INTR_DISABLE,
                .timer_sel = LEDC_TIMER_0,
        };
        ledc_channel_config(&ledcConfig);

        uint32_t maxDuty = (1 << LEDC_TIMER_10_BIT) - 1;
        int increament = 0;

        do {
                for (uint32_t duty=0; duty <= maxDuty; duty += 10) {
                        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
                        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                        vTaskDelay(pdMS_TO_TICKS(20));
                }
                for (uint32_t duty=maxDuty; duty >= 0; duty -= 10) {
                        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
                        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                        vTaskDelay(pdMS_TO_TICKS(20));
                }

                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                vTaskDelay(pdMS_TO_TICKS(500));

                increament++;
        } while (increament<100);
}
