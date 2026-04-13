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

project(ch32_sdk C ASM)

# ====================== PROJECT SELECT ======================
if(NOT DEFINED PROJECT_DIR)
    message(FATAL_ERROR "PROJECT_DIR не задан! Запускай через wch.py")
endif()

if(NOT EXISTS ${PROJECT_DIR})
    message(FATAL_ERROR "PROJECT_DIR не существует: ${PROJECT_DIR}")
endif()

message(STATUS "Building project: ${PROJECT_DIR}")

# ====================== COMMON FLAGS ======================
set(COMMON_COMPILE_FLAGS
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

# ====================== SDK (STATIC LIB) ======================
set(SDK_SOURCES
    Src/ch32v20x_adc.c
    Src/ch32v20x_bkp.c
    Src/ch32v20x_can.c
    Src/ch32v20x_crc.c
    Src/ch32v20x_dbgmcu.c
    Src/ch32v20x_dma.c
    Src/ch32v20x_exti.c
    Src/ch32v20x_flash.c
    Src/ch32v20x_gpio.c
    Src/ch32v20x_i2c.c
    Src/ch32v20x_it.c
    Src/ch32v20x_iwdg.c
    Src/ch32v20x_misc.c
    Src/ch32v20x_opa.c
    Src/ch32v20x_pwr.c
    Src/ch32v20x_rcc.c
    Src/ch32v20x_rtc.c
    Src/ch32v20x_spi.c
    Src/ch32v20x_tim.c
    Src/ch32v20x_usart.c
    Src/ch32v20x_wwdg.c
    Src/core_riscv.c
    Src/system_ch32v20x.c
    Src/debug.c
    Inc/startup_ch32v20x_D6.S
)

add_library(ch32_sdk STATIC ${SDK_SOURCES})

target_include_directories(ch32_sdk PUBLIC
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Src
)

target_compile_options(ch32_sdk PUBLIC ${COMMON_COMPILE_FLAGS})

target_compile_definitions(ch32_sdk PUBLIC
    CH32V20X
    SYSCLK_FREQ_144MHz_HSE
)

# ====================== PROJECT ======================
set(PROJECT_FILE "${PROJECT_DIR}/src.cmake")
if(NOT EXISTS ${PROJECT_FILE})
    message(FATAL_ERROR "Нет src.cmake в ${PROJECT_DIR}")
endif()

include(${PROJECT_FILE})
list(TRANSFORM SOURCES PREPEND "${PROJECT_DIR}/")

get_filename_component(PROJECT_NAME ${PROJECT_DIR} NAME)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${PROJECT_DIR})
target_link_libraries(${PROJECT_NAME}.elf ch32_sdk)

target_compile_options(${PROJECT_NAME}.elf PRIVATE ${COMMON_COMPILE_FLAGS})

# ====================== LINK ======================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Src/Link.ld")

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -mabi=ilp32
    -lc
    -lgcc
    -Wl,--no-warn-mismatch          # обязательно для nixpkgs newlib
)

# ====================== POST BUILD ======================
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
)

add_custom_target(size
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(flash
    COMMAND wlink flash ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(erase
    COMMAND wlink erase
)
