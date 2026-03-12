#include "Timer.h"

TaskHandle_t displayTaskControl;
ssd1306_handle_t displayControl;
i2c_master_bus_handle_t busControl;
static const char *timerTag = "TIMER";
static const char *displayTag = "I2C DISPLAY";
esp_err_t err;


void DisplayInit(){
        ESP_LOGI(displayTag, "Инициализация шины i2c");
        i2c_master_bus_config_t busConfig = {
                .clk_source = I2C_CLK_SRC_DEFAULT,
                .glitch_ignore_cnt = 7, 
                .i2c_port = I2C_NUM_0,
                .scl_io_num = sclPin,
                .sda_io_num = sdaPin,
                .flags.enable_internal_pullup = true,
        };
        vTaskDelay(pdMS_TO_TICKS(500));
        err = i2c_new_master_bus(&busConfig, &busControl);
        if (err != ESP_OK) {
                ESP_LOGE(displayTag, "Ошибка инициализации шины");
                return;
        }

        ssd1306_config_t displayConfig = {
                .display_enabled = true,
                .i2c_address = I2C_SSD1306_DEV_ADDR,
                .i2c_clock_speed = 400000,
                .panel_size = SSD1306_PANEL_128x64,
                .offset_x = 0,
                .flip_enabled = false,
        };
        vTaskDelay(pdMS_TO_TICKS(500));
        ssd1306_init(busControl, &displayConfig, &displayControl);
}


void BeaperInit(){
        gpio_set_level(beaperPin, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(beaperPin, 0);
}

void DisplayTimer(void *arg){
    uint32_t tryCounter = 0;
    static int ticks = 950;
    char timeStr[16];

    for (int i = timerSeconds; i >= 0; i--)
    {
        ssd1306_clear_display(displayControl, false);

        int minute = i / 60;
        int seconds = i % 60;

        snprintf(timeStr, sizeof(timeStr), "0%d:%02d", minute, seconds);

        ssd1306_display_text_x3(displayControl, 2, timeStr, false);
        BeaperInit();

        if (xTaskNotifyWait(0, ULLONG_MAX, &tryCounter, pdMS_TO_TICKS(10)) == pdPASS) {
                ticks -= 450;
                if (ticks <= 0) {
                        goto boom;
                }
        }

        vTaskDelay(pdMS_TO_TICKS(ticks));
    }

boom:
        lcdClear();
        lcdSetText("YOU LOSE", 4, 0);
        lcdSetText("KABOOM!!", 4, 1);

    while (1) {
        gpio_set_level(beaperPin, 1);
    }
}
