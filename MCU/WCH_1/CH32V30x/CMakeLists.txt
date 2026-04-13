cmake_minimum_required(VERSION 3.20)

# ====================== TOOLCHAIN ======================
# ====================== TOOLCHAIN ======================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

project(ch32v30_sdk C ASM)

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

# ====================== COMMON FLAGS (CH32V30x) ======================
set(COMMON_COMPILE_FLAGS
    -march=rv32imafc_zicsr
    -mabi=ilp32f
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
    Inc/startup_ch32v30x_D8C.S
)

add_library(ch32_sdk STATIC ${SDK_SOURCES})

target_include_directories(ch32_sdk PUBLIC
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Src
)

target_compile_options(ch32_sdk PUBLIC ${COMMON_COMPILE_FLAGS})

target_compile_definitions(ch32_sdk PUBLIC
    CH32V30X
    SYSCLK_FREQ_144MHz_HSE
)

# ====================== PROJECT SOURCES ======================
set(PROJECT_FILE "${PROJECT_DIR}/src.cmake")

if(EXISTS ${PROJECT_FILE})
    include(${PROJECT_FILE})
    message(STATUS "Используется src.cmake из проекта")
else()
    # Если src.cmake нет — ищем main.c
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
    -mabi=ilp32f
    -lc
    -lgcc
    -Wl,--no-warn-mismatch
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
