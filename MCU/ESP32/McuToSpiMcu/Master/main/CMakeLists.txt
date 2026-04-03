idf_component_register(
        SRCS 
        "main.c" 
        "Spi/Spi.c"

        INCLUDE_DIRS "." 

        REQUIRES 
        driver 
        freertos 
        esp_driver_spi)
