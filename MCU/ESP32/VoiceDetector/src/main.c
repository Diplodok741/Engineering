// #include "driver/gpio.h"
// #include "hal/gpio_types.h"
// #include <stdbool.h>
//
// void app_main() {
//   gpio_set_direction(15, GPIO_MODE_INPUT);
//   gpio_set_direction(2, GPIO_MODE_OUTPUT);
//   gpio_set_direction(4, GPIO_MODE_OUTPUT);
//
//   int counter = 0;
//   bool statusOne = false;
//   bool statusTwo = false;
//
//   while (1) {
//     gpio_set_level(2, statusOne);
//     gpio_set_level(4, statusTwo);
//     if ((gpio_get_level(15))) {
//       counter++;
//       switch (counter) {
//         case 0: statusTwo = statusOne = false; break;
//         case 1: statusOne = !statusOne; break;
//         case 2: statusTwo = !statusTwo; break;
//         case 3: counter = 0; break;
//       }
//     }
//   }
// }


#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include "esp_timer.h"

#define DEBOUNCE_DELAY_MS 200

void app_main() {
    gpio_set_direction(15, GPIO_MODE_INPUT);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_direction(4, GPIO_MODE_OUTPUT);
    
    int counter = 0;
    bool statusOne = false;
    bool statusTwo = false;
    int64_t last_trigger_time = 0;

    gpio_set_level(2, statusOne);
    gpio_set_level(4, statusTwo);

    while (1) {
        // Проверка с антидребезгом
        if (gpio_get_level(15) && 
            (esp_timer_get_time() - last_trigger_time) > DEBOUNCE_DELAY_MS * 1000) {
            
            last_trigger_time = esp_timer_get_time();
            counter++;
            
            switch (counter % 3) {  // Цикл по 3 состояниям
                case 1: 
                    statusOne = true;
                    statusTwo = false;
                    break;
                case 2: 
                    statusOne = false;
                    statusTwo = true;
                    break;
                case 0: 
                    statusOne = false;
                    statusTwo = false;
                    break;
            }
            
            gpio_set_level(2, statusOne);
            gpio_set_level(4, statusTwo);
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Снижаем нагрузку на CPU
    }
}
