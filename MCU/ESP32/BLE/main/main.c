#include <stdio.h>
#include <esp_bt.h>
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


esp_err_t errBLE;
static const char *tagBLE = "BLE";

void app_main(void)
{

}

void BLEInit(){
        ESP_LOGI(tagBLE, "Начало инициализации BLE");
}
