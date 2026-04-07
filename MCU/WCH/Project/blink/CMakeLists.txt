cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)

project(ch32v203_firmware C ASM)

# Отключаем тесты CMake
set(CMAKE_C_COMPILER_WORKS 1)

# ==================== Флаги ====================
add_compile_options(
    -march=rv32imac_zicsr
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    -Os
)

# ==================== Исходники ====================
set(SOURCES
    main.c

    # WCH Peripheral Library
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

    # Core + system + debug
    ../../Src/core_riscv.c
    ../../Src/system_ch32v20x.c
    ../../Src/debug.c

    # Только этот startup — он предоставляет _start
    ../../Inc/startup_ch32v20x_D6.S
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

# ==================== Линкер ====================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/../../Src/Link.ld")

add_link_options(
    -T${LINKER_SCRIPT}
    -nostartfiles
    -nostdlib
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
    --specs=nosys.specs
    -mabi=ilp32                  # ← важно для линковки
)

# ==================== Пост-обработка ====================
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .hex / .bin / .lst"
)

add_custom_target(flash
    COMMAND openocd -f interface/wlink.cfg -f target/ch32v20x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing firmware"
    VERBATIM
)
