#include <stdint.h>
#include <stdio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <stdlib.h>
#include <wchar.h>
#include "driver/uart.h"
#include "esp_err.h"
#include "hal/uart_types.h"
#include "portmacro.h"

#define uartRXPin 3
#define uartTXPin 1

static const char *tagUart = "UART";
esp_err_t err;

void UartInit();
void SendData(void *arg);

void app_main(void)
{
        UartInit();
        ESP_LOGI(tagUart, "Создание задачи приемо-передачи UART");
        xTaskCreate(SendData, "UART SendRead", 2048, NULL, 10, NULL);
        if(err != ESP_OK){
                ESP_LOGE(tagUart, "Ошибка создания задачи");
                return;
        }
        ESP_LOGI(tagUart, "UART задача создана");
}

void UartInit(){
        ESP_LOGI(tagUart, "Настройка конфигурации UART со следующими параметрами: \n\tскорость передачи данных: 115200\n\tколичество бит: 8\n\tбит паритета: отключен\n\tуправление потоком: отключено\n\tисточник частоты: стандартный\n\tстоповый бит: 1");
        uart_config_t uartConfig = {
                .baud_rate = 115200,
                .data_bits = UART_DATA_8_BITS,
                .parity = UART_PARITY_DISABLE,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                .source_clk = UART_SCLK_DEFAULT,
                .stop_bits = UART_STOP_BITS_1,
        };
        ESP_LOGI(tagUart, "Установка драйвера UART...");
        err = uart_driver_install(UART_NUM_0, 1024 * 2, 1024 * 2, 0, NULL, 0);
        if(err!=ESP_OK){
                ESP_LOGE(tagUart, "Ошибка установки драйвера UART");
                return;
        }
                ESP_LOGI(tagUart, "УСПЕШНА");

        ESP_LOGI(tagUart, "Применение конфигурации UART");
        err = uart_param_config(UART_NUM_0, &uartConfig);
        if(err!=ESP_OK){
                ESP_LOGE(tagUart, "Ошибка настройки конфигурации UART");
                return;
        }
        ESP_LOGI(tagUart, "УСПЕШНА");

        ESP_LOGI(tagUart, "Установка пинов интерфейса: TX = %d, RX = %d...", 17, 16);
        err = uart_set_pin(UART_NUM_0, 17, 16, -1, -1);
        if(err!=ESP_OK){
                ESP_LOGE(tagUart, "Ошибка установки пинов UART");
                return;
        }
        ESP_LOGI(tagUart, "УСПЕШНА");
}

void SendData(void *arg){
        ESP_LOGI(tagUart, "Выделение динамической памяти для данных UART");
        uint8_t *data = (uint8_t*)malloc(1024);
        if(data == NULL){
                ESP_LOGE(tagUart, "Ошибка выделения динамической памяти для данных UART");
                vTaskDelete(NULL);
                return;
        }

        ESP_LOGI(tagUart, "Задача UART запущена. Ожидание данных...");
        while (1) {
                int readSendData = uart_read_bytes(UART_NUM_0, data, 1024-1, 20/portTICK_PERIOD_MS);

                if(readSendData > 0){
                        data[readSendData] = '\0';
                        ESP_LOGI(tagUart, "Получены %d данные: '%s'", readSendData, (char*)data);
                        uart_write_bytes(UART_NUM_0, (const char*)data, readSendData);
                        ESP_LOGI(tagUart, "Отправлены %d данные", readSendData);
                }
        }
        free(data);
        vTaskDelete(NULL);
}
