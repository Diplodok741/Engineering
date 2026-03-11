#include "Spi.h"

static const char *tagSpi = "SPI Slave";
static esp_err_t err;

void SpiInit(){
        ESP_LOGI(tagSpi, "Инициализация SPI Slave...");
        spi_bus_config_t busConfig = {
                .quadhd_io_num = -1,
                .quadwp_io_num = -1,
                .miso_io_num = slaveMisoPin,
                .mosi_io_num = slaveMosiPin,
                .sclk_io_num = slaveSclkPin,
                .max_transfer_sz = 64,
        };
        spi_slave_interface_config_t slaveConfig = {
                .mode = 0,
                .queue_size = 7,
                .spics_io_num = slaveChipSelectPin,
        };

        ESP_LOGI(tagSpi, "Инициализация устройства SPI...");
        err = spi_slave_initialize(SPI3_HOST, &busConfig, &slaveConfig, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(err);
        if(err != ESP_OK){
                ESP_LOGW(tagSpi, "НЕУДАЧНА");
                return;
        }
        ESP_LOGI(tagSpi, "УСПЕШНА");

        ESP_LOGI(tagSpi, "Инициализация SPI Slave проша УСПЕШНО");
}

uint8_t SpiTransaction(){
        spi_slave_transaction_t slaveTransaction;
        memset(&slaveTransaction, 0, sizeof(slaveTransaction));

        uint8_t recievedData;

        slaveTransaction.length = sizeof(recievedData) * 8;
        slaveTransaction.rx_buffer = &recievedData;

        spi_slave_transmit(SPI3_HOST, &slaveTransaction, portMAX_DELAY);

        return recievedData;
}
