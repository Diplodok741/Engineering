#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c_master.h>
#include <driver/i2c_types.h>
#include "hal/i2c_types.h"
#include "hal/spi_types.h"
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>


#define sdaPin 23
#define sclPin 21
#define frequency 400000
#define address 0x3a


static const char *tag = "I2C";
esp_err_t err;
i2c_master_bus_handle_t masterControl;
i2c_master_dev_handle_t deviceControl;


void I2cInit(){
        ESP_LOGI(tag, "Инициализация мастера I2C");
        i2c_master_bus_config_t masterConfig = {
                .scl_io_num = sclPin,
                .sda_io_num = sdaPin,
                .i2c_port = I2C_NUM_1,
                .clk_source = frequency,
                .glitch_ignore_cnt = 7,
                .flags.enable_internal_pullup = true,
        };
        err = i2c_new_master_bus(&masterConfig, &masterControl);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "ОШИБКА");
                return;
        }
        ESP_LOGI(tag, "УСПЕШНА");

        ESP_LOGI(tag, "Инициализация переферийного устройства");
        i2c_device_config_t deviceConfig = {
                .dev_addr_length = I2C_ADDR_BIT_LEN_7,
                .device_address = address,
                .scl_speed_hz = 400000,
        };
        err = i2c_master_bus_add_device(masterControl, &deviceConfig, &deviceControl);
        if (err != ESP_OK) {
                ESP_LOGE(tag, "ОШИБКА");
                return;
        }
        ESP_LOGI(tag, "УСПЕШНА");
}

void I2cTransaction(){
}
