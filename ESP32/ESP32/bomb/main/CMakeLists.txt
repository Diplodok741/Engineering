idf_component_register(SRCS "main.c" "Timer.c" "Password.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos driver esp_timer esp_lcd)
