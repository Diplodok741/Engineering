#include "spi.h"

static const char *tag = "SPI";
esp_err_t err;
spi_device_handle_t spiControl;

void SpiInit(){
        ESP_LOGI(tag, "Инициализация шины SPI...");
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

        ESP_LOGI(tag, "Настройка переферийного устройства...");
        spi_device_interface_config_t slaveConfig = {
                .mode = 3,
                .queue_size = 7,
                .clock_speed_hz = 250000,
                .flags = SPI_DEVICE_BIT_LSBFIRST,
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
        memset(&transaction, 0, sizeof(transaction));

        transaction.length = 128;

        err = spi_device_transmit(spiControl, &transaction);
        if(err != ESP_OK){
                ESP_LOGW(tag, "Получение данных невозможна");
                return ;
        }

        // ESP_LOGI(tag, "Передана команда %x", command);
        for (int i=0; i<5; i++) {
                // ESP_LOGI(tag, "Полученны данные %x", markId[i]);
        }
}
