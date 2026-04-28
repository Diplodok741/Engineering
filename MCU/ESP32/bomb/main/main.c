#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "Pinout.h"
#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <wchar.h>
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "esp_rtc_time.h"
#include "Password.h"
#include "Timer.h"
#include "time.h"
#include <1602_driver.h>

static const char *tag = "MAIN";
static const char *sensorTag = "SENSOR";

gpio_config_t gpioOutput = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL<<greenLed)|(1ULL<<trigerPin)|(1ULL<<beaperPin)|(1ULL<<brightnessPin)|(1ULL<<columnThree)|(1ULL<<columnTwo)|(1ULL << columnOne)),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
};
gpio_config_t gpioInput = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((1ULL<<echoPin)|(1ULL<<rowOne)|(1ULL<<rowTwo)|(1ULL<<rowThree)|(1ULL<<rowFour)),
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
};

float SensorGetDistance(){
        gpio_set_level(trigerPin, 1);
        esp_rom_delay_us(10);
        gpio_set_level(trigerPin, 0);

        uint64_t start = 0;
        while (gpio_get_level(echoPin) == 0) {
                start = esp_timer_get_time(); // текущее состояние 
                                              // esp_timer_get_time получает микросекунды с момента загрузи чипа
                                              // защита от зависания
                                              // если прошло больше чем полсекунды то выходим
                if (esp_timer_get_time() - start > 500000) break;  
        }

        uint64_t pulseBegin = esp_timer_get_time();
        while (gpio_get_level(echoPin) == 1) {
                if (esp_timer_get_time() - pulseBegin > 500000) break;
        }

        uint64_t pulseEnd = esp_timer_get_time();
        uint64_t pulseDuration = pulseEnd - pulseBegin;

        float distance = pulseDuration * 0.0343f/2.0f;

        // минимальный импульс 10 мкс а максимальный 450        
        if (pulseDuration < 10 || distance > 450) {
            ESP_LOGE(sensorTag, "Нет сигнала или объект слишком далеко\n");
        } else {
                ESP_LOGI(sensorTag, "Расстояние: %.1f cm", distance);
        }
        
        return distance;
        vTaskDelay(pdMS_TO_TICKS(1000));
}


void app_main(void) {
        gpio_config(&gpioOutput);
        gpio_config(&gpioInput);
        gpio_set_level(greenLed, 0);

        DisplayInit();
        lcdInit();

        ssd1306_clear_display(displayControl, false);
        gpio_set_level(brightnessPin, 0);

        float distance = 200;

        while (distance >= 100) {
                distance = SensorGetDistance();
                vTaskDelay(pdMS_TO_TICKS(1000));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        
        xTaskCreate(DisplayTimer, "Display Timer", 4096, NULL, 2, &displayTaskControl); 
        xTaskCreate(TaskLcd, "Lcd Defuse Code", 4096, NULL, 0, &lcdTaskControl);
        xTaskCreate(ButtonMatrix, "Button enter password", 4096, NULL, 5, &buttonTaskControl);
}
