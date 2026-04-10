cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
set(CMAKE_C_COMPILER riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====================== Проект ======================
project(ch32v30x_firmware LANGUAGES C ASM)

set(CMAKE_C_COMPILER_WORKS 1)

# ====================== Пути ======================
set(BASE_DIR ${CMAKE_SOURCE_DIR}/../..)
set(INC_DIR ${BASE_DIR}/Inc/V30X)
set(SRC_DIR ${BASE_DIR}/Src/V30X)

# ====================== Флаги компиляции ======================
add_compile_options(
    -march=rv32imafdc_zicsr_zifencei
    -mabi=ilp32d
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    -Wextra
    -Os
    -g
)
add_compile_definitions(
    CH32V30X
    SYSCLK_FREQ_144MHz_HSE
    HSE_VALUE=8000000
)

# ====================== Исходники ======================
set(SOURCES
    ${CMAKE_SOURCE_DIR}/main.c

    ${SRC_DIR}/ch32v30x_adc.c
    ${SRC_DIR}/ch32v30x_bkp.c
    ${SRC_DIR}/ch32v30x_can.c
    ${SRC_DIR}/ch32v30x_crc.c
    ${SRC_DIR}/ch32v30x_dbgmcu.c
    ${SRC_DIR}/ch32v30x_dma.c
    ${SRC_DIR}/ch32v30x_exti.c
    ${SRC_DIR}/ch32v30x_flash.c
    ${SRC_DIR}/ch32v30x_gpio.c
    ${SRC_DIR}/ch32v30x_i2c.c
    ${SRC_DIR}/ch32v30x_it.c
    ${SRC_DIR}/ch32v30x_iwdg.c
    ${SRC_DIR}/ch32v30x_misc.c
    ${SRC_DIR}/ch32v30x_opa.c
    ${SRC_DIR}/ch32v30x_pwr.c
    ${SRC_DIR}/ch32v30x_rcc.c
    ${SRC_DIR}/ch32v30x_rtc.c
    ${SRC_DIR}/ch32v30x_spi.c
    ${SRC_DIR}/ch32v30x_tim.c
    ${SRC_DIR}/ch32v30x_usart.c
    ${SRC_DIR}/ch32v30x_wwdg.c

    ${SRC_DIR}/core_riscv.c
    ${SRC_DIR}/system_ch32v30x.c
    ${SRC_DIR}/debug.c

    # Startup
    ${INC_DIR}/startup_ch32v30x_D8C.S
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${INC_DIR}
    ${SRC_DIR}
)

# ====================== Линковщик ======================
set(LINKER_SCRIPT ${SRC_DIR}/Link.ld)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    --specs=nosys.specs        # Убрали nano.specs — он не работает в твоём toolchain
    -mabi=ilp32d
    -lc
    -lnosys
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# ====================== Пост обработка ======================
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Создание .hex / .bin / .lst"
)

# ====================== Дополнительные команды ======================

add_custom_target(flash
    COMMAND wlink flash ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Прошивка через wlink..."
    VERBATIM
)

add_custom_target(erase
    COMMAND wlink erase
    COMMENT "Полное стирание чипа"
    VERBATIM
)
add_custom_target(size
    COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Размер прошивки"
)
