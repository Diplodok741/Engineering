#include "Display.h"

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
}

void LvglInit(){
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

void Scale(){
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 30, 100);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_mode(scale, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_obj_center(scale);

    lv_scale_set_total_tick_count(scale, 21);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_scale_set_range(scale, 0, 100);

    static const char * custom_labels[] = {"0 °C", "25 °C", "50 °C", "75 °C", "100 °C", NULL};
    lv_scale_set_text_src(scale, custom_labels);

    static lv_style_t indicator_style;
    lv_style_init(&indicator_style);

    /* Label style properties */
    lv_style_set_text_font(&indicator_style, LV_FONT_DEFAULT);
    lv_style_set_text_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));

    /* Major tick properties */
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
    /* Main line properties */
    lv_style_set_line_color(&main_line_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_line_width(&main_line_style, 2U); // Tick width
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

    /* Add a section */
    static lv_style_t section_minor_tick_style;
    static lv_style_t section_label_style;
    static lv_style_t section_main_line_style;

    lv_style_init(&section_label_style);
    lv_style_init(&section_minor_tick_style);
    lv_style_init(&section_main_line_style);

    /* Label style properties */
    lv_style_set_text_font(&section_label_style, LV_FONT_DEFAULT);
    lv_style_set_text_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));

    lv_style_set_line_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_label_style, 5U); /*Tick width*/

    lv_style_set_line_color(&section_minor_tick_style, lv_palette_lighten(LV_PALETTE_RED, 2));
    lv_style_set_line_width(&section_minor_tick_style, 4U); /*Tick width*/

    /* Main line properties */
    lv_style_set_line_color(&section_main_line_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_main_line_style, 4U); /*Tick width*/

    /* Configure section styles */
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

void FillScreen(uint16_t color){
        uint16_t *bufColor = heap_caps_malloc(128*3, MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
        for (int i=0; i<129; i++) {
                bufColor[i] = color;
        }
        for(int i=0; i<129; i++){
                esp_lcd_panel_draw_bitmap(displayControl, 0, i, 129, i+1, bufColor);
        }
        heap_caps_free(bufColor);
}

void ScrollLabel()
{
    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                      "and wrap long text automatically.");
    lv_obj_set_width(label1, 75);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 75);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
}

void Line()
{
    /*Create an array for the points of the line*/
    static lv_point_precise_t line_points[] = { {5, 5}, {7, 7}, {12, 1}, {18, 6}, {24, 1} };

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 3);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(lv_screen_active());
    lv_line_set_points(line1, line_points, 5);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_center(line1);
}
