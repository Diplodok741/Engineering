cmake_minimum_required(VERSION 3.20)

# ====================== TOOLCHAIN ======================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

set(CMAKE_C_COMPILER riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
set(CMAKE_OBJCOPY riscv32-none-elf-objcopy)
set(CMAKE_OBJDUMP riscv32-none-elf-objdump)
set(CMAKE_SIZE riscv32-none-elf-size)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

project(ch32v203_firmware C ASM)

# ====================== ОБЩИЕ ФАЙЛЫ ======================
set(LIB_SOURCES
    Src/ch32v30x_adc.c
    Src/ch32v30x_bkp.c
    Src/ch32v30x_can.c
    Src/ch32v30x_crc.c
    Src/ch32v30x_dbgmcu.c
    Src/ch32v30x_dma.c
    Src/ch32v30x_exti.c
    Src/ch32v30x_flash.c
    Src/ch32v30x_gpio.c
    Src/ch32v30x_i2c.c
    Src/ch32v30x_it.c
    Src/ch32v30x_iwdg.c
    Src/ch32v30x_misc.c
    Src/ch32v30x_opa.c
    Src/ch32v30x_pwr.c
    Src/ch32v30x_rcc.c
    Src/ch32v30x_rtc.c
    Src/ch32v30x_spi.c
    Src/ch32v30x_tim.c
    Src/ch32v30x_usart.c
    Src/ch32v30x_wwdg.c
    Src/core_riscv.c
    Src/system_ch32v30x.c
    Src/debug.c
    Inc/startup_ch32v30x_D8.S
)

# ====================== ОПРЕДЕЛЕНИЕ ТЕКУЩЕГО ПРОЕКТА ======================
get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src.cmake")
    set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PROJECT_SRC_FILE "${PROJECT_DIR}/src.cmake")
else()
    set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/Project/blink")
    set(PROJECT_SRC_FILE "${PROJECT_DIR}/src.cmake")
endif()

if(NOT EXISTS ${PROJECT_SRC_FILE})
    message(FATAL_ERROR "Файл src.cmake не найден в ${PROJECT_DIR}")
endif()

include(${PROJECT_SRC_FILE})

list(TRANSFORM SOURCES PREPEND "${PROJECT_DIR}/")

list(APPEND SOURCES ${LIB_SOURCES})

# ====================== ФЛАГИ ======================
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
    -Wno-attributes
)

add_compile_definitions(
    CH32V20X
    SYSCLK_FREQ_144MHz_HSE
)

# ====================== TARGET ======================
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Src
    ${PROJECT_DIR}
)

# ====================== LINKER & POST BUILD ======================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Src/Link.ld")

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    -nostdlib
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
    --specs=nosys.specs
    -mabi=ilp32
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Генерация .hex / .bin / .lst"
)

add_custom_target(size
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(flash
    COMMAND wlink flash ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}.elf
    VERBATIM
)

add_custom_target(erase
    COMMAND wlink erase
    VERBATIM
)
