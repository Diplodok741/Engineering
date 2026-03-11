#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

static const char *tag = "MATRIX";

gpio_config_t gpioInput = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_INPUT,
    .pull_down_en = GPIO_PULLDOWN_ENABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pin_bit_mask = ((1ULL << 2) | (1ULL << 16) | (1ULL << 4) | (1ULL << 15)),
};

gpio_config_t gpioOutput = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pin_bit_mask = ((1ULL << 5) | (1ULL << 18) | (1ULL << 19) | (1ULL << 22)),
};

void ButtonMatrix() {
  char columnNumber[3] = {5, 18, 19};
  char rowNumber[4] = {15, 21, 4, 16};

  char buttonStatus[3][4] = {
      {' ', '7', '4', '1'},
      {'0', '8', '5', '2'},
      {'#', '9', '6', '3'},
  };

  bool rowStatus = false;
  bool led = false;

  while (1) {
    for (int i = 0; i < 3; i++) {
      gpio_set_level(columnNumber[i], 1);
      for (int j = 0; j < 4; j++) {
        rowStatus = gpio_get_level(rowNumber[j]);
        if (rowStatus) {
          ESP_LOGI(tag, "Нажата кнопка %c", buttonStatus[i][j]);
          gpio_set_level(22, !led);
        }
      }
      gpio_set_level(columnNumber[i], 0);
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

void app_main(void) {
  gpio_config(&gpioInput);
  gpio_config(&gpioOutput);

  ButtonMatrix();
}
