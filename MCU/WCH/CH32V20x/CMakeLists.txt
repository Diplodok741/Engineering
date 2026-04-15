cmake_minimum_required(VERSION 3.20)

# ====================== TOOLCHAIN (MounRiverStudio) ======================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

set(TOOLCHAIN_ROOT "/home/blank/Engineering/MCU/WCH/toolchain/riscv32")
set(TOOLCHAIN_PREFIX "${TOOLCHAIN_ROOT}/bin/riscv32-wch-elf-")

set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_SIZE "${TOOLCHAIN_PREFIX}size")
set(CMAKE_AR "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_RANLIB "${TOOLCHAIN_PREFIX}ranlib")

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

project(ch32v20_sdk C ASM)

message(STATUS "Toolchain root: ${TOOLCHAIN_ROOT}")
message(STATUS "Toolchain prefix: ${TOOLCHAIN_PREFIX}")
message(STATUS "Using GCC: ${CMAKE_C_COMPILER}")
message(STATUS "Using objcopy: ${CMAKE_OBJCOPY}")

# ====================== PROJECT SELECT ======================
if(NOT DEFINED PROJECT_DIR)
    get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
  
    if(CURRENT_DIR MATCHES "/Project/([^/]+)$")
        set(PROJECT_DIR "${CURRENT_DIR}")
        message(STATUS "PROJECT_DIR определён автоматически: ${PROJECT_DIR}")
    else()
        set(TRY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
        if(EXISTS "${TRY_DIR}/src.cmake" OR EXISTS "${TRY_DIR}/main.c")
            set(PROJECT_DIR "${TRY_DIR}")
            message(STATUS "PROJECT_DIR найден через fallback: ${PROJECT_DIR}")
        else()
            message(FATAL_ERROR "PROJECT_DIR не задан! Запускайте через wch.py build")
        endif()
    endif()
endif()

if(NOT EXISTS ${PROJECT_DIR})
    message(FATAL_ERROR "PROJECT_DIR не существует: ${PROJECT_DIR}")
endif()

message(STATUS "Building project: ${PROJECT_DIR}")

# ====================== COMMON FLAGS (CH32V20x) ======================
set(COMMON_COMPILE_FLAGS
    -march=rv32imac
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

# ====================== SDK ======================
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
    SYSCLK_FREQ_48MHz_HSE     # ← часто используется для V20x
)

# ====================== PROJECT SOURCES ======================
set(PROJECT_FILE "${PROJECT_DIR}/src.cmake")
if(EXISTS ${PROJECT_FILE})
    include(${PROJECT_FILE})
    message(STATUS "Используется src.cmake из проекта")
else()
    if(EXISTS "${PROJECT_DIR}/main.c")
        set(SOURCES "main.c")
        message(STATUS "src.cmake не найден → используем main.c")
    else()
        message(FATAL_ERROR "Не найден ни src.cmake, ни main.c в ${PROJECT_DIR}")
    endif()
endif()

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
