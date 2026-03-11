#include "Display.h"
#include "Spi.h"
#include "core/lv_obj_tree.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "lv_api_map_v8.h"
#include "misc/lv_timer.h"
#include <stdint.h>

static const char *tag = "appMain";

void app_main(void)
{
        SpiInit();
        DisplayInit();
        LvglInit();

        // lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

        // lvgl_port_lock(0);
        // lv_obj_clean(lv_screen_active());
        // lvgl_port_unlock();
        // Scale();
        FillScreen(0x0ff0);
        // uint8_t data;
        
        while (1) {
                lv_timer_handler();
                vTaskDelay(pdMS_TO_TICKS(5));

                // data = SpiTransaction();
                // lvgl_port_lock(0);
                // if(data == 0x10){
                //         ESP_LOGI(tag, "Полученны данные %d", data);
                //         lv_obj_clean(lv_screen_active());
                //         lvgl_port_lock(0);
                //         Scale();
                //         lvgl_port_unlock();
                // } else {
                //         ESP_LOGI(tag, "Полученны данные %d", data);
                //         lv_obj_clean(lv_screen_active());
                //         lvgl_port_lock(0);
                //         Line();
                //         lvgl_port_unlock();
                // }
                // // lvgl_port_unlock();
        }
}
