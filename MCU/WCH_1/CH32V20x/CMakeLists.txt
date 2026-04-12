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

# ====================== SDK (STATIC LIB) ======================
set(SDK_SOURCES
        Src/libc.c
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
    Inc/startup_ch32v20x_D8.S
)

add_library(ch32_sdk STATIC ${SDK_SOURCES})

target_include_directories(ch32_sdk PUBLIC
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Src
)

set(FLOAT_ABI_FLAGS
    -mfloat-abi=soft          # или softfp, если вдруг понадобится
    -mabi=ilp32               # уже есть, но на всякий случай дублируем
)

target_compile_options(ch32_sdk PUBLIC
        # ${FLOAT_ABI_FLAGS}
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

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${PROJECT_DIR}
)

target_link_libraries(${PROJECT_NAME}.elf ch32_sdk)

# ====================== LINK ======================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Src/Link.ld")

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    # -nostdlib
    # -nodefaultlibs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nano.specs
    --specs=nosys.specs
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







# cmake_minimum_required(VERSION 3.20)
#
# # ====================== TOOLCHAIN ======================
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_SYSTEM_PROCESSOR riscv32)
#
# set(CMAKE_C_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_OBJCOPY riscv32-none-elf-objcopy)
# set(CMAKE_OBJDUMP riscv32-none-elf-objdump)
# set(CMAKE_SIZE riscv32-none-elf-size)
#
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_ASM_COMPILER_WORKS 1)
#
# # ====================== ПРОЕКТ ======================
# project(ch32v20x_firmware C ASM)
#
# # ====================== ОПРЕДЕЛЕНИЕ ТЕКУЩЕГО ПРОЕКТА ======================
#
# # 1. Если пользователь явно указал проект
# if(DEFINED PROJECT_NAME)
#     set(CURRENT_PROJECT ${PROJECT_NAME})
# # 2. Если запустили из папки Project/xxx
# elseif(CMAKE_CURRENT_SOURCE_DIR MATCHES "/Project/([^/]+)$")
#     set(CURRENT_PROJECT ${CMAKE_MATCH_1})
# # 3. Если запустили из корня — используем blink по умолчанию (как в wch.py)
# else()
#     set(CURRENT_PROJECT "blink")
# endif()
#
# set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/Project/${CURRENT_PROJECT}")
#
# if(NOT EXISTS "${PROJECT_DIR}/src.cmake")
#     message(FATAL_ERROR "
#         Не найден файл src.cmake для проекта '${CURRENT_PROJECT}'!
#         Ожидаемый путь: ${PROJECT_DIR}/src.cmake
#     ")
# endif()
#
# message(STATUS "Собираем проект: ${CURRENT_PROJECT}")
# message(STATUS "Папка проекта: ${PROJECT_DIR}")
#
# # ====================== ИСХОДНИКИ ======================
#
# # Общие библиотечные файлы (HAL + startup + core)
# set(LIB_SOURCES
#     Src/ch32v20x_adc.c
#     Src/ch32v20x_bkp.c
#     Src/ch32v20x_can.c
#     Src/ch32v20x_crc.c
#     Src/ch32v20x_dbgmcu.c
#     Src/ch32v20x_dma.c
#     Src/ch32v20x_exti.c
#     Src/ch32v20x_flash.c
#     Src/ch32v20x_gpio.c
#     Src/ch32v20x_i2c.c
#     Src/ch32v20x_it.c
#     Src/ch32v20x_iwdg.c
#     Src/ch32v20x_misc.c
#     Src/ch32v20x_opa.c
#     Src/ch32v20x_pwr.c
#     Src/ch32v20x_rcc.c
#     Src/ch32v20x_rtc.c
#     Src/ch32v20x_spi.c
#     Src/ch32v20x_tim.c
#     Src/ch32v20x_usart.c
#     Src/ch32v20x_wwdg.c
#     Src/core_riscv.c
#     Src/system_ch32v20x.c
#     Src/debug.c
#
#     # Startup (выбираем нужный, можно сделать через src.cmake потом)
#     Inc/startup_ch32v20x_D8.S
# )
#
# # Подключаем src.cmake текущего проекта
# include("${PROJECT_DIR}/src.cmake")
#
# # Превращаем относительные пути в абсолютные
# list(TRANSFORM SOURCES PREPEND "${PROJECT_DIR}/")
#
# # Объединяем всё
# list(APPEND SOURCES ${LIB_SOURCES})
#
# # ====================== КОМПИЛЯЦИЯ ======================
#
# add_compile_options(
#     -march=rv32imac_zicsr
#     -mabi=ilp32
#     -mcmodel=medany
#     -msmall-data-limit=8
#     -mno-save-restore
#     -ffunction-sections
#     -fdata-sections
#     -fno-common
#     -Wall
#     -Os
#     -Wno-attributes
# )
#
# add_compile_definitions(
#     CH32V20X
#     SYSCLK_FREQ_144MHz_HSE
# )
#
# add_executable(${CURRENT_PROJECT}.elf ${SOURCES})
#
# target_include_directories(${CURRENT_PROJECT}.elf PRIVATE
#     ${CMAKE_SOURCE_DIR}/Inc
#     ${CMAKE_SOURCE_DIR}/Src
#     ${PROJECT_DIR}
# )
#
# # ====================== ЛИНКЕР И ПОСТ-БИЛД ======================
#
# set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Src/Link.ld")
#
# target_link_options(${CURRENT_PROJECT}.elf PRIVATE
#     -T${LINKER_SCRIPT}
#     -nostartfiles
#     -nostdlib
#     -Wl,--gc-sections
#     -Wl,--print-memory-usage
#     --specs=nano.specs
#     --specs=nosys.specs
#     -mabi=ilp32
# )
#
# add_custom_command(TARGET ${CURRENT_PROJECT}.elf POST_BUILD
#     COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${CURRENT_PROJECT}.elf> ${CURRENT_PROJECT}.hex
#     COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${CURRENT_PROJECT}.elf> ${CURRENT_PROJECT}.bin
#     COMMAND ${CMAKE_OBJDUMP} -S -d $<TARGET_FILE:${CURRENT_PROJECT}.elf> > ${CURRENT_PROJECT}.lst
#     COMMENT "Генерация .hex / .bin / .lst для ${CURRENT_PROJECT}"
# )
#
# add_custom_target(size
#     COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${CURRENT_PROJECT}.elf>
#     DEPENDS ${CURRENT_PROJECT}.elf
# )
#
# add_custom_target(flash
#     COMMAND wlink flash ${CURRENT_PROJECT}.bin
#     DEPENDS ${CURRENT_PROJECT}.elf
#     VERBATIM
# )
#
# add_custom_target(erase
#     COMMAND wlink erase
#     VERBATIM
# )
#



