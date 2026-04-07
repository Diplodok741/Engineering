cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ==================== Компилятор ====================
set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)

project(ch32v203_firmware C ASM)

set(CMAKE_C_COMPILER_WORKS 1)

# ==================== Архитектура ====================
set(MCU_FLAGS
    -march=rv32imac_zicsr
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
)

# ==================== Оптимизация ====================
set(COMMON_FLAGS
    ${MCU_FLAGS}
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    -Os
)

# ==================== Исходники ====================
set(SOURCES
    main.c

    ../../Src/ch32v20x_adc.c
    ../../Src/ch32v20x_bkp.c
    ../../Src/ch32v20x_can.c
    ../../Src/ch32v20x_crc.c
    ../../Src/ch32v20x_dbgmcu.c
    ../../Src/ch32v20x_dma.c
    ../../Src/ch32v20x_exti.c
    ../../Src/ch32v20x_flash.c
    ../../Src/ch32v20x_gpio.c
    ../../Src/ch32v20x_i2c.c
    ../../Src/ch32v20x_it.c
    ../../Src/ch32v20x_iwdg.c
    ../../Src/ch32v20x_misc.c
    ../../Src/ch32v20x_opa.c
    ../../Src/ch32v20x_pwr.c
    ../../Src/ch32v20x_rcc.c
    ../../Src/ch32v20x_rtc.c
    ../../Src/ch32v20x_spi.c
    ../../Src/ch32v20x_tim.c
    ../../Src/ch32v20x_usart.c
    ../../Src/ch32v20x_wwdg.c

    ../../Src/core_riscv.c
    ../../Src/system_ch32v20x.c
    ../../Src/debug.c

    ../../Inc/startup_ch32v20x_D8W.S
)

# ==================== Таргет ====================
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ../../Inc
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE ${COMMON_FLAGS})

# ==================== Линковка ====================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/../../Src/Link.ld")

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}

    -T${LINKER_SCRIPT}

    -nostdlib
    -nostartfiles

    -Wl,--gc-sections
    -Wl,--print-memory-usage

    -lgcc   # только это оставить можно
)

# ==================== Пост-обработка ====================
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND riscv32-none-elf-objcopy -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND riscv32-none-elf-objcopy -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND riscv32-none-elf-objdump -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating HEX / BIN / LST"
)

# ==================== Прошивка ====================
add_custom_target(flash
    COMMAND openocd -f interface/wlink.cfg -f target/ch32v20x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
)
