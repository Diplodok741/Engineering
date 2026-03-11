#ifndef DISPLAY_H
#define DISPLAY_H

#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <esp_err.h>
#include "driver/spi_common.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_st7735.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_types.h"
#include "esp_lvgl_port.h"
#include "esp_lvgl_port_disp.h"
#include "hal/spi_types.h"
#include "misc/lv_types.h"
#include "soc/clk_tree_defs.h"
#include "lvgl.h"
#include "stdio.h"
#include "esp_log.h"
#include <stdint.h>

#define spiDisplayClkPin 14
#define spiDisplayMosiPin 13
#define spiDisplayCSPin 15
#define displayDataCommandPin 2
#define displayResetPin 4

void DisplayInit();
void Scale();
void ButtonStruct();

#endif
