#include <math.h>
#include "Timer.h"
#include "1602_driver.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "Pinout.h"
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>
#include <freertos/task.h>
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "esp_rtc_time.h"

extern TaskHandle_t lcdTaskControl;
extern TaskHandle_t buttonTaskControl;
extern TaskHandle_t notifyControl;

void ButtonMatrix(void *arg);
void TaskLcd(void *arg);
