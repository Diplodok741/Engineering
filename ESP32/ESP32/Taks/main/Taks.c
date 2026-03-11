#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include <driver/gpio.h>
#include <portmacro.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>

#undef PORTMACRO_H

// #define taskHelloStekSize 4096
//
// TaskHandle_t taskHelloControl = NULL;
//
// void HelloTask(void *arg){
//         while(1){
//                 printf("Задача запущена\n");
//                 printf("helloWorld\n");
//                 vTaskDelay(pdMS_TO_TICKS(1000));     // Приостановка задачи на 1 секунду 
//         }
// }
//
// void app_main(void)
// {
//         xTaskCreate(HelloTask, "Hello Task", taskHelloStekSize, NULL, 10, &taskHelloControl);
// }

#define taskOneStekSize 2048
#define taskTwoStekSize 2048

TaskHandle_t taskOneCoreProControl = NULL;
TaskHandle_t taskTwoCoreAppControl = NULL;

void TaskCoreOne(void *arg){
        while (1) {
                printf("Запущена первая задача на ядре %d\n", xPortGetCoreID());
                vTaskDelay(pdMS_TO_TICKS(1000));
        }
}

void TaskCoreTwo(void *arg){
        while (1) {
                printf("Запущена вторая задача на ядре %d\n", xPortGetCoreID());
                vTaskDelay(pdMS_TO_TICKS(1000));
        }
}

void app_main(){
        xTaskCreatePinnedToCore(TaskCoreOne, "One Task on first Core", taskOneStekSize, NULL, 5, &taskOneCoreProControl, 0);
        xTaskCreatePinnedToCore(TaskCoreTwo, "Two Task on second Core", taskTwoStekSize, NULL, 5, &taskTwoCoreAppControl, 1);
}

// TaskHandle_t taskGpioControl = NULL;
//
// void Gpio(){
//         esp_rom_gpio_pad_select_gpio(22);
//         gpio_set_direction(22, GPIO_MODE_OUTPUT);
//
//         while (1) {
//                 gpio_set_level(22, 1);
//                 vTaskDelay(pdMS_TO_TICKS(1000));
//                 gpio_set_level(22, 0);
//                 vTaskDelay(pdMS_TO_TICKS(1000));
//         }
// }
//
// void app_main(){
//         xTaskCreate(Gpio, "Gpio Blink Task", 4096, NULL, 1, &taskGpioControl);
// }

