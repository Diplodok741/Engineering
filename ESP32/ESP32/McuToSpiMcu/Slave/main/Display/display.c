#include "display.h"
#include "Spi/spi.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "lv_conf_internal.h"
#include "misc/lv_area.h"
#include "misc/lv_types.h"
#include "widgets/button/lv_button.h"
#include "widgets/label/lv_label.h"
#include <stdio.h>
#include <string.h>

esp_lcd_panel_io_handle_t displayPinsControl = NULL;
esp_lcd_panel_handle_t displayControl = NULL;
static const char* tagDisplay = "ST7735";
esp_err_t err;

void DisplayInit(){
        ESP_LOGI(tagDisplay, "Инициализация Дисплея");
        spi_bus_config_t busConfig = {
                .sclk_io_num = spiDisplayClkPin,
                .mosi_io_num = spiDisplayMosiPin,
                .quadhd_io_num = -1,
                .quadwp_io_num = -1,
                .max_transfer_sz = 128 * 128 * sizeof(uint16_t),
        };
        err = spi_bus_initialize(SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO);
        if(err != ESP_OK){
                ESP_LOGE(tagDisplay, "ОШИБКА SPI");
                return;
        }

        esp_lcd_panel_io_spi_config_t displayPinsConfig = {
                .dc_gpio_num = displayDataCommandPin,
                .cs_gpio_num = spiDisplayCSPin,
                .pclk_hz = 40000000,
                .lcd_cmd_bits = 8,
                .lcd_param_bits = 8,
                .spi_mode = 0,
                .trans_queue_depth = 10,
        };
        err = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &displayPinsConfig, &displayPinsControl);
        if(err != ESP_OK){
                ESP_LOGE(tagDisplay, "ОШИБКА Настройки пинов");
                return;
        }

        esp_lcd_panel_dev_config_t displayConfig = {
                .reset_gpio_num = displayResetPin,
                .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
                .bits_per_pixel = 16,
        };
        err = esp_lcd_new_panel_st7735(displayPinsControl, &displayConfig, &displayControl);
        if(err != ESP_OK){
                ESP_LOGE(tagDisplay, "ОШИБКА добавления дисплея");
                return;
        }
        ESP_LOGI(tagDisplay, "УСПЕШНА");

        esp_lcd_panel_reset(displayControl);
        esp_lcd_panel_init(displayControl);
        esp_lcd_panel_disp_on_off(displayControl, true);

         ESP_LOGI(tagDisplay, "Инициализация LVGL...");
        const lvgl_port_cfg_t lvglConfig = ESP_LVGL_PORT_INIT_CONFIG();
        err = lvgl_port_init(&lvglConfig);
        static lv_display_t *lvglControl;

        const lvgl_port_display_cfg_t lvglDisplayConfig = {
                .io_handle = displayPinsControl,
                .panel_handle = displayControl,
                .buffer_size = 128 * 128,
                .double_buffer = true,
                .hres = 129,
                .vres = 129,
                .monochrome = false, 
                .flags = {
                        .swap_bytes = true,
                        .buff_dma = true,
                        .buff_spiram = false,
                        .direct_mode = false, 
                        .full_refresh = false,
                },
        };
        lvglControl = lvgl_port_add_disp(&lvglDisplayConfig);

        ESP_LOGI(tagDisplay, "УСПЕШНА");
}


void ButtonStruct(){
        struct stringData data;

        lv_obj_t *buttonValue = lv_button_create(lv_screen_active());
        lv_obj_t *labelValue = lv_label_create(buttonValue);
        lv_obj_align(buttonValue, LV_ALIGN_TOP_RIGHT, -10, 0);
        lv_obj_set_size(buttonValue, 50, 30);
        lv_obj_set_style_text_font(labelValue, &lv_font_montserrat_14,  0);
        lv_obj_center(labelValue);
        lv_label_set_text(labelValue, "50");
        // lv_label_set_text(labelValue, data.value);


        lv_obj_t *buttonDegree = lv_button_create(lv_screen_active());
        lv_obj_t *labelDegree = lv_label_create(buttonDegree);
        lv_obj_align(buttonDegree, LV_ALIGN_RIGHT_MID, -10, 0);
        lv_obj_set_size(buttonDegree, 50, 30);
        lv_obj_set_style_text_font(labelDegree, &lv_font_montserrat_14,  0);
        lv_obj_center(labelDegree);
        // lv_label_set_text(labelDegree, "2");
        lv_label_set_text(labelDegree, data.degree);

        lv_obj_t *buttonUnit = lv_button_create(lv_screen_active());
        lv_obj_t *labelUnit = lv_label_create(buttonUnit);
        lv_obj_align(buttonUnit, LV_ALIGN_BOTTOM_RIGHT, -10, 0);
        lv_obj_set_size(buttonUnit, 50, 30);
        lv_obj_set_style_text_font(labelUnit, &lv_font_montserrat_14,  0);
        lv_obj_center(labelUnit);
        lv_label_set_text(labelUnit, "v");
        // lv_label_set_text(labelUnit, data.unit);
}

void Scale(){
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 30, 100);
    lv_obj_align(scale, LV_ALIGN_LEFT_MID, 10, 0);
    // lv_obj_set_pos(scale, -60, 0);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_mode(scale, LV_SCALE_MODE_VERTICAL_RIGHT);

    lv_scale_set_total_tick_count(scale, 21);
    lv_scale_set_major_tick_every(scale, 10);

    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, 1, LV_PART_ITEMS);
    lv_scale_set_range(scale, 0, 100);

    static const char *custom_labels[] = { NULL};
    lv_scale_set_text_src(scale, custom_labels);

    static lv_style_t indicator_style;
    lv_style_init(&indicator_style);

    lv_style_set_text_font(&indicator_style, LV_FONT_DEFAULT);

    lv_style_set_line_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_width(&indicator_style, 10U);      /*Tick length*/
    lv_style_set_line_width(&indicator_style, 2U);  /*Tick width*/
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

    static lv_style_t minor_ticks_style;
    lv_style_init(&minor_ticks_style);
    lv_style_set_line_color(&minor_ticks_style, lv_palette_lighten(LV_PALETTE_BLUE, 2));
    lv_style_set_width(&minor_ticks_style, 5U);         /*Tick length*/
    lv_style_set_line_width(&minor_ticks_style, 2U);    /*Tick width*/
    lv_obj_add_style(scale, &minor_ticks_style, LV_PART_ITEMS);

    static lv_style_t main_line_style;
    lv_style_init(&main_line_style);
    lv_style_set_line_color(&main_line_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_line_width(&main_line_style, 2U); // Tick width
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

    static lv_style_t section_minor_tick_style;
    static lv_style_t section_label_style;
    static lv_style_t section_main_line_style;

    lv_style_init(&section_label_style);
    lv_style_init(&section_minor_tick_style);
    lv_style_init(&section_main_line_style);

    lv_style_set_text_font(&section_label_style, LV_FONT_DEFAULT);
    lv_style_set_text_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));

    lv_style_set_line_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_label_style, 5U); /*Tick width*/

    lv_style_set_line_color(&section_minor_tick_style, lv_palette_lighten(LV_PALETTE_RED, 2));
    lv_style_set_line_width(&section_minor_tick_style, 4U); /*Tick width*/

    lv_style_set_line_color(&section_main_line_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_main_line_style, 4U); /*Tick width*/

    lv_scale_section_t * section = lv_scale_add_section(scale);
    lv_scale_set_section_range(scale, section, 75, 100);
    lv_scale_set_section_style_indicator(scale, section, &section_label_style);
    lv_scale_set_section_style_items(scale, section, &section_minor_tick_style);
    lv_scale_set_section_style_main(scale, section, &section_main_line_style);

    lv_obj_set_style_bg_color(scale, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
    lv_obj_set_style_bg_opa(scale, LV_OPA_50, 0);
    lv_obj_set_style_pad_left(scale, 8, 0);
    lv_obj_set_style_radius(scale, 8, 0);
    lv_obj_set_style_pad_ver(scale, 20, 0);
}
