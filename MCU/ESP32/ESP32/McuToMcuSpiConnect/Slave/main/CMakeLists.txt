idf_component_register(SRCS "Slave.c" "Display.c" "Spi.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_spi freertos driver esp_lcd )
