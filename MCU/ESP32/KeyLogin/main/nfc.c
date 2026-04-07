#include "nfc.h"
#include "driver/spi_master.h"
#include "xtensa/hal.h"
#include <stdint.h>

static const char *tag = "NFC";
esp_err_t err;
spi_device_handle_t spiControl;


void SpiInit(){
        ESP_LOGI(tag, "Инициализация шины SPI");
        spi_bus_config_t busConfig = {
                .miso_io_num = miso,
                .mosi_io_num = mosi,
                .sclk_io_num = sclk,
                .quadhd_io_num = -1,
                .quadwp_io_num = -1,
                .max_transfer_sz = 256,
        };
        err = spi_bus_initialize(SPI3_HOST, &busConfig, SPI_DMA_CH_AUTO);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "Шина не иницаилизирована");
        }
        ESP_LOGI(tag, "Шина инициализирована");

        ESP_LOGI(tag, "Добавление периферийного устройства");
        spi_device_interface_config_t deviceConfig = {
                .clock_speed_hz = 100000,
                .mode = 0,
                .spics_io_num = chipSelect,
                .queue_size = 7,
        };
        err = spi_bus_add_device(SPI3_HOST, &deviceConfig, &spiControl);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "Устройство не добавлено");
        }
        ESP_LOGI(tag, "Устройство добавлено");
}


void SpiTransaction(){
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        uint8_t markId[5];
        uint8_t command[3] = {0x26, 0x93, 0x20};

        transaction.length = 128;
        transaction.rx_buffer = &markId;
        transaction.tx_buffer = &command;

        err = spi_device_transmit(spiControl, &transaction);
        if(err != ESP_OK){
                ESP_LOGW(tag, "Получение данных невозможна");
                return ;
        }

        ESP_LOGI(tag, "Передана команда %x", command);
        for (int i=0; i<5; i++) {
                ESP_LOGI(tag, "Полученны данные %x", markId[i]);
        }
        // return *markId;
}
