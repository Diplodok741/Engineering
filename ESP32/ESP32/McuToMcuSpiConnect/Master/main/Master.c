#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <string.h>

#define spiChipSelectPin 5
#define spiMosiPin 23
#define spiMisoPin 19
#define spiSclkPin 18


void SpiInit();
// void SpiTransaction(void *data);
void SpiTransaction(uint8_t data);


static const char *tagSpi = "SPI Master";
esp_err_t err;
spi_device_handle_t spiControl = NULL;
// TaskHandle_t taskSpi = NULL;


void app_main(void)
{
        SpiInit();
        uint8_t dataOne = 0x00;
        uint8_t dataTwo = 0x10;

        // xTaskCreate(SpiTransaction, "Spi Master Transaction", 4096, &dataOne, 5, &taskSpi);

        while (1) {
                SpiTransaction(dataOne);
                vTaskDelay(pdMS_TO_TICKS(5000));
                SpiTransaction(dataTwo);
                vTaskDelay(pdMS_TO_TICKS(5000));
        }

}

void SpiInit(){
        ESP_LOGI(tagSpi, "Инициализация интерфейса SPI...");
        spi_bus_config_t busConfig = {
                .miso_io_num = spiMisoPin,
                .mosi_io_num = spiMosiPin,
                .sclk_io_num = spiSclkPin,
                .quadhd_io_num = -1, 
                .quadwp_io_num = -1,
                .max_transfer_sz = 64,
        };

        err = spi_bus_initialize(SPI3_HOST, &busConfig, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(err);
        if(err != ESP_OK){
                ESP_LOGW(tagSpi, "НЕУДАЧНA");
        };
        ESP_LOGI(tagSpi, "УСПЕШНА");

        ESP_LOGI(tagSpi, "Добавление устройства к шине SPI");
        spi_device_interface_config_t slaveConfig = {
                .mode = 0,
                .spics_io_num = spiChipSelectPin,
                .queue_size = 7,
                .clock_speed_hz = 10000000,
        };

        err = spi_bus_add_device(SPI3_HOST, &slaveConfig, &spiControl);
        if(err != ESP_OK){
                ESP_LOGW(tagSpi, "НЕУДАЧНA");
        };
        ESP_LOGI(tagSpi, "УСПЕШНА");
}

void SpiTransaction(uint8_t data){
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        transaction.length = sizeof(data) * 8;
        transaction.tx_buffer = &data;

        err = spi_device_transmit(spiControl, &transaction);
        ESP_ERROR_CHECK(err);
        if(err != ESP_OK){
                ESP_LOGW(tagSpi, "Передача данных невозможна");
                return;
        }
        ESP_LOGI(tagSpi, "Передача данных %d ...", data);

        // while (1) {
        //         vTaskDelay(pdMS_TO_TICKS(1000));
        // }
}
