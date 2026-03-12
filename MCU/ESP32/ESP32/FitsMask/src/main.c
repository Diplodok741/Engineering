#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "esp_err.h"
#include "hal/i2s_types.h"
#include "soc/clk_tree_defs.h"
#include <stddef.h>
#include <stdint.h>
#include "sampleaac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "portmacro.h"

#define bclkPin 26
#define wordSelectPin 25
#define dataPin 22

void app_main() {
  i2s_chan_handle_t i2sChannelControl = NULL;    // Создание дескриптора передачи
  i2s_chan_config_t i2sChannelConfig = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);    // Настройка канала I2S с помощью готового шаблона
  ESP_ERROR_CHECK(i2s_new_channel(&i2sChannelConfig, &i2sChannelControl, NULL));    // Создание нового настроенного канала и привязка с дескриптором передачи

  // Настройка интерфейса I2S
  i2s_std_config_t i2sConfig = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),    // Макрос для настройки частоты
    .gpio_cfg = {
      .bclk = bclkPin,
      .dout = dataPin,
      .din = I2S_GPIO_UNUSED,
      .ws = wordSelectPin,
    },
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
    // .slot_cfg = {
    //   .slot_mode = I2S_SLOT_MODE_MONO,
    //   .slot_mask = I2S_STD_SLOT_RIGHT,
    //   .msb_right = true,
    //   .slot_bit_width = 16,
    //   .data_bit_width = 16,
    //   .ws_width = 1,
    //   .bit_shift = false,
    //   .ws_pol = true,
    // },
  };
  i2s_channel_init_std_mode(i2sChannelControl, &i2sConfig);    // Инициализация I2S в стандартном режиме и привязка к дескриптору передачи
  i2s_channel_enable(i2sChannelControl);    // Включение канала I2S
  
  // Буфер со звуковыми данными 
  // const unsigned char mBufferVoice[] = {
  //   0xFF, 0xF9, 0x5C, 0x80, 0x2E, 0x11, 0x22, 0x33, 0x55, 0x00, 0x99, 0xaa, 0xff, 0x00, 0xF2,
  // };

  const uint16_t test[] = {0xff, 0xff};
  const uint16_t test2[] = {0x00, 0x00};

  i2s_channel_write(i2sChannelControl, test, sizeof(test), NULL, 1000);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  i2s_channel_write(i2sChannelControl, test2, sizeof(test2), NULL, 1000);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  i2s_channel_write(i2sChannelControl, test, sizeof(test), NULL, 1000);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  i2s_channel_disable(i2sChannelControl);
}
