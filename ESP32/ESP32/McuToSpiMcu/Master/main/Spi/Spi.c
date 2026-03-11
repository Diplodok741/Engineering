#include "Spi.h"
#include <string.h>

static const char* tagSpi = "SPI Master";
esp_err_t spiErr;
spi_device_handle_t spiControl = NULL;


struct measuredValues {
        struct flags {
                bool resistanceValue;
                bool buttonState;
                bool brightnessControl;
        };
        char unit[8];
        float value;
        uint8_t degree;
}__attribute__((packed));
static DRAM_ATTR struct measuredValues measured __attribute__((aligned(4)));

void SpiInit(){
        logInfo(tagSpi, "Начало инициализации SPI");
        logInfo(tagSpi, "Инициалиазция Шины SPI...");
        spi_bus_config_t busConfig = {
                .miso_io_num = spiMisoPin, 
                .mosi_io_num = spiMosiPin,
                .sclk_io_num = spiSclkPin, 
                .quadhd_io_num = -1,
                .quadwp_io_num = -1, 
        };
        spiErr = spi_bus_initialize(SPI3_HOST, &busConfig, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(spiErr);
        if(spiErr != ESP_OK){
                logError(tagSpi, "ОШИБКА");
                return;
        };
        logInfo(tagSpi, "УСПЕШНА...");

        logInfo(tagSpi, "Добавление устройства к шине SPI...");
        spi_device_interface_config_t slaveConfig = {
                .mode = 0,
                .spics_io_num = spiChipSelectPin,
                .queue_size = 7,
                .clock_speed_hz = 10000000,
        };
        spiErr = spi_bus_add_device(SPI3_HOST, &slaveConfig, &spiControl);
        ESP_ERROR_CHECK(spiErr);
        if(spiErr != ESP_OK){
                logError(tagSpi, "ОШИБКА");
                return;
        }
        logInfo(tagSpi, "УСПЕШНО");
        logInfo(tagSpi, "Инициализация SPI завершена успешно");
}

void SpiTransaction(uint8_t value, uint8_t degree, char* unit){
        memset(&measured, 0, sizeof(measured));
        measured.value = value;
        measured.degree = degree;
        strncpy(measured.unit, unit, sizeof(measured.unit)-1);

        spi_transaction_t t;
        logInfo(tagSpi, "Начальная иницаилизация структуры транзакции нулевыми значениями для избавления от мусора");
        memset(&t, 0, sizeof(t));

        t.length = sizeof(measured) * 8;
        t.tx_buffer = &measured;

        logInfo(tagSpi, "Начало передачи данных");
        spiErr = spi_device_transmit(spiControl, &t);
        ESP_ERROR_CHECK(spiErr);
        if(spiErr != ESP_OK){
                logError(tagSpi, "Передача данных невозможна");
                return;
        }
        logInfo(tagSpi, "Передача данных\n\tЗначение: %d\n\tСтепень: %d\n\tЕдиница измерения: %s\n Прошла успешно", measured.value, measured.degree, measured.unit);
}


