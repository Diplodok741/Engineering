cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====================== PROJECT ======================
project(ch32v30x_firmware C ASM)

set(CMAKE_C_COMPILER_WORKS 1)

# ====================== PATHS ======================
set(BASE_DIR ${CMAKE_SOURCE_DIR}/../..)
set(INC_DIR ${BASE_DIR}/Inc/V30X)
set(SRC_DIR ${BASE_DIR}/Src/V30X)

# ====================== COMPILER FLAGS ======================
add_compile_options(
    -march=rv32imafdc_zicsr_zifencei     # ← добавили "d" после "f"
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

add_compile_definitions(CH32V30X)

# ====================== SOURCES ======================
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
    ${INC_DIR}/startup_ch32v30x_D8.S
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${INC_DIR}
    ${SRC_DIR}
)

# ====================== LINKER ======================
set(LINKER_SCRIPT ${SRC_DIR}/Link.ld)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    --specs=nano.specs
    --specs=nosys.specs
    -mabi=ilp32d          
    -lc
    -lnosys
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# ====================== POST BUILD ======================
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Generating .hex / .bin / .lst"
)

# ====================== CUSTOM TARGETS ======================
add_custom_target(size
    COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Firmware size"
)

add_custom_target(flash
    COMMAND wlink flash ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing with wlink..."
    VERBATIM
)
