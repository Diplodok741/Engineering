#include "spi.h"

static const char *tagSpi = "SPI Slave";
static esp_err_t err;
QueueHandle_t queueSpiTransacrionControl = NULL;

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

struct stringData SpiTransaction(){
        static DRAM_ATTR struct stringData dataRecieved __attribute__((aligned(4)));

        struct measuredValues{
                char unit[8];
                uint8_t value;
                uint8_t degree;
        }__attribute__((packed));
        static DRAM_ATTR struct measuredValues measure __attribute__((aligned(4)));

        spi_slave_transaction_t slaveTransaction;
        memset(&slaveTransaction, 0, sizeof(slaveTransaction));

        slaveTransaction.length = sizeof(measure) * 8;
        slaveTransaction.rx_buffer = &measure;

        ESP_LOGI(tagSpi, "Получение данных");
        spi_slave_transmit(SPI3_HOST, &slaveTransaction, portMAX_DELAY);

        ESP_LOGI(tagSpi, "Преобразование полученных данных в строки");
        snprintf(dataRecieved.value, sizeof(char)*8, "%d", measure.value);
        snprintf(dataRecieved.degree, sizeof(char)*8, "%d", measure.degree);
        strcpy(dataRecieved.unit, measure.unit); 

        ESP_LOGI(tagSpi, "Получены данные: \n\tЗначение: %s, \n\tСтепень: %s, \n\tЕдиница измерения: %s", dataRecieved.value, dataRecieved.degree, dataRecieved.unit);

        return dataRecieved;
}


void TaskSpiTransaction(void *parameters){
        ESP_LOGI(tagSpi, "Запуск задачи передачи данных по SPI");
        while (1) {
                ESP_LOGI(tagSpi, "Выполнение транзакции");
                struct stringData dataRecieved = SpiTransaction();

                ESP_LOGI(tagSpi, "Создание очереди на 10 элементов");

        }
}

