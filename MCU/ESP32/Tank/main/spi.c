#include <freertos/FreeRTOS.h>
#include "driver/spi_common.h"
#include "hal/spi_types.h"
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

#define mosiPin 12
#define misoPin 1
#define sclkPin 14
#define csPin   23

static const char *tag = "SPI";
esp_err_t err;
spi_device_handle_t spiControl;

void SpiInit(){
        ESP_LOGI(tag, "Инициализация шины SPI");
        spi_bus_config_t busConfig = {
                .max_transfer_sz = 32,
                .miso_io_num = misoPin,
                .mosi_io_num = mosiPin,
                .sclk_io_num = sclkPin,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
        };
        err = spi_bus_initialize(SPI1_HOST, &busConfig, SPI_DMA_CH_AUTO);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "ОШИБКА");
                return;
        }
        ESP_LOGI(tag, "УСПЕШНО");

        ESP_LOGI(tag, "Настройка переферийного устройства");
        spi_device_interface_config_t slaveConfig = {
                .mode = 0,
                .spics_io_num = csPin,
                .queue_size = 7,
                .clock_speed_hz = 10000000,
        };
        err = spi_bus_add_device(SPI1_HOST, &slaveConfig, &spiControl);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "ОШИБКА");
                return;
        }
        ESP_LOGI(tag, "УСПЕШНО");

        ESP_LOGI(tag, "Инициализация SPI завершена");
}


void SpiTransaction(){
        spi_transaction_t transaction;
}
