#include "esp_err.h"
#include "string.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "driver/i2c_types.h"
#include "driver/i2c.h"
#include "hal/i2c_types.h"
#include "1602_driver.h"
#include "soc/clk_tree_defs.h"
#include <stdbool.h>
#include <stdio.h>
#include <ssd1306.h>
#include "Pinout.h"
#include <string.h>
#include "driver/gpio.h"

#define timerSeconds 300
#define addresDisplay 0x3C

extern TaskHandle_t displayTaskControl;
extern ssd1306_handle_t displayControl;

void DisplayInit();
void BeaperInit();
void DisplayTimer(void *arg);
