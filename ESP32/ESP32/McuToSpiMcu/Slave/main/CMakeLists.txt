idf_component_register(
        SRCS 
        "main.c"
        "Display/display.c"
        "Spi/spi.c"
        
        INCLUDE_DIRS "."

        REQUIRES
        driver
        freertos
        esp_lcd
        esp_driver_spi)
