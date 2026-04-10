cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)

project(ch32v30_firmware C ASM)

set(CMAKE_C_COMPILER_WORKS 1)

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

set(SOURCES
    main.c

    ../../Src/ch32v30x_adc.c
    ../../Src/ch32v30x_bkp.c
    ../../Src/ch32v30x_can.c
    ../../Src/ch32v30x_crc.c
    ../../Src/ch32v30x_dbgmcu.c
    ../../Src/ch32v30x_dma.c
    ../../Src/ch32v30x_exti.c
    ../../Src/ch32v30x_flash.c
    ../../Src/ch32v30x_gpio.c
    ../../Src/ch32v30x_i2c.c
    ../../Src/ch32v30x_it.c
    ../../Src/ch32v30x_iwdg.c
    ../../Src/ch32v30x_misc.c
    ../../Src/ch32v30x_opa.c
    ../../Src/ch32v30x_pwr.c
    ../../Src/ch32v30x_rcc.c
    ../../Src/ch32v30x_rtc.c
    ../../Src/ch32v30x_spi.c
    ../../Src/ch32v30x_tim.c
    ../../Src/ch32v30x_usart.c
    ../../Src/ch32v30x_wwdg.c

    ../../Src/core_riscv.c
    ../../Src/system_ch32v30x.c
    ../../Src/debug.c

    ../../Inc/startup_ch32v30x_D8.S
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE ../../Inc ../../Src)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/../../Src/Link.ld")

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
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Generating .hex / .bin / .lst"
)

add_custom_target(size
    COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Firmware size"
)

add_custom_target(flash
    COMMAND openocd -f interface/wlink.cfg -f target/ch32v30x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing firmware"
    VERBATIM
)










# cmake_minimum_required(VERSION 3.20)
#
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# set(CMAKE_SYSTEM_PROCESSOR riscv32)
#
# # Компиляторы из твоего devShell (riscv32-none-elf-*)
# set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
# set(CMAKE_CXX_COMPILER riscv32-none-elf-g++)
# set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
#
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#
# # ====================== Проект ======================
# project(ch32v30x_firmware LANGUAGES C ASM CXX)
#
# # Заставляем CMake считать компилятор рабочим
# set(CMAKE_C_COMPILER_WORKS   1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
#
# # ====================== Пути ======================
# get_filename_component(BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
# set(INC_DIR ${BASE_DIR}/Inc/)
# set(SRC_DIR ${BASE_DIR}/Src/)
#
# # ====================== Флаги компиляции ======================
# add_compile_options()
# add_compile_options(
#         -ffreestanding 
#         -nostdlib
#     -march=rv32imafdc
#     -mabi=ilp32d
#     -mcmodel=medany
#     -msmall-data-limit=8
#     -mno-save-restore
#     -ffunction-sections
#     -fdata-sections
#     -fno-common
#     -Wall
#     -Wextra
#     -Os
#     -g3
# )
#
# add_compile_definitions(
#     CH32
#     SYSCLK_FREQ_144MHz_HSE
#     HSE_VALUE=8000000
# )
#
# # ====================== Исходники ======================
# set(SOURCES
#     ${CMAKE_SOURCE_DIR}/main.c
#     ${SRC_DIR}/ch32v30x_adc.c
#     ${SRC_DIR}/ch32v30x_bkp.c
#     ${SRC_DIR}/ch32v30x_can.c
#     ${SRC_DIR}/ch32v30x_crc.c
#     ${SRC_DIR}/ch32v30x_dbgmcu.c
#     ${SRC_DIR}/ch32v30x_dma.c
#     ${SRC_DIR}/ch32v30x_exti.c
#     ${SRC_DIR}/ch32v30x_flash.c
#     ${SRC_DIR}/ch32v30x_gpio.c
#     ${SRC_DIR}/ch32v30x_i2c.c
#     ${SRC_DIR}/ch32v30x_it.c
#     ${SRC_DIR}/ch32v30x_iwdg.c
#     ${SRC_DIR}/ch32v30x_misc.c
#     ${SRC_DIR}/ch32v30x_opa.c
#     ${SRC_DIR}/ch32v30x_pwr.c
#     ${SRC_DIR}/ch32v30x_rcc.c
#     ${SRC_DIR}/ch32v30x_rtc.c
#     ${SRC_DIR}/ch32v30x_spi.c
#     ${SRC_DIR}/ch32v30x_tim.c
#     ${SRC_DIR}/ch32v30x_usart.c
#     ${SRC_DIR}/ch32v30x_wwdg.c
#     ${SRC_DIR}/core_riscv.c
#     ${SRC_DIR}/system_ch32v30x.c
#     ${SRC_DIR}/debug.c
#
#     # Startup
#     ${INC_DIR}/startup_ch32v30x_D8C.S
# )
#
# add_executable(${PROJECT_NAME}.elf ${SOURCES})
#
# target_include_directories(${PROJECT_NAME}.elf PRIVATE
#     ${INC_DIR}
#     ${SRC_DIR}
# )
#
# # ====================== Линковщик (с newlib-nano) ======================
# set(LINKER_SCRIPT ${SRC_DIR}/Link.ld)
#
# target_link_options(${PROJECT_NAME}.elf PRIVATE
#     -T${LINKER_SCRIPT}
#     -nostartfiles
#     --specs=nosys.specs     # ← stub-ы для системных вызовов
#     -mabi=ilp32d
#     -Wl,--gc-sections
#     -Wl,--print-memory-usage
# )
#
# # ====================== Пост-обработка ======================
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#     COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
#     COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
#     COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
#     COMMENT "Generating .hex / .bin / .lst files"
# )
#
# # ====================== Дополнительные цели ======================
# add_custom_target(flash
#     COMMAND wlink flash ${PROJECT_NAME}.bin
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Flashing with wlink..."
#     VERBATIM
# )
#
# add_custom_target(erase
#     COMMAND wlink erase
#     COMMENT "Erasing chip with wlink"
#     VERBATIM
# )
#
# add_custom_target(size
#     COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Firmware size"
# )







# cmake_minimum_required(VERSION 3.20)
#
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# set(CMAKE_SYSTEM_PROCESSOR riscv32)
# set(CMAKE_C_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_CXX_COMPILER riscv32-none-elf-g++)
# set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${CMAKE_SYSROOT}")
#
# # ====================== Проект ======================
# project(ch32v30x_firmware LANGUAGES C ASM)
#
# set(CMAKE_C_COMPILER_WORKS 1)
#
# # ====================== Пути ======================
# get_filename_component(BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
# # set(BASE_DIR ${CMAKE_SOURCE_DIR}/../..)
# set(INC_DIR ${BASE_DIR}/Inc/)
# set(SRC_DIR ${BASE_DIR}/Src/)
#
# # ====================== Флаги компиляции ======================
# add_compile_options(
#     -march=rv32imafdc
#     -mabi=ilp32d
#     -mcmodel=medany
#     -msmall-data-limit=8
#     -mno-save-restore
#     -ffunction-sections
#     -fdata-sections
#     -fno-common
#     -Wall
#     -Wextra
#     -Os
#     -g
# )
#
# add_compile_definitions(
#     CH32
#     -DSYSCLK_FREQ_144MHz_HSE
#     -DHSE_VALUE=8000000
# )
#
# # ====================== Исходники ======================
# set(SOURCES
#     ${CMAKE_SOURCE_DIR}/main.c
#
#     ${SRC_DIR}/ch32v30x_adc.c
#     ${SRC_DIR}/ch32v30x_bkp.c
#     ${SRC_DIR}/ch32v30x_can.c
#     ${SRC_DIR}/ch32v30x_crc.c
#     ${SRC_DIR}/ch32v30x_dbgmcu.c
#     ${SRC_DIR}/ch32v30x_dma.c
#     ${SRC_DIR}/ch32v30x_exti.c
#     ${SRC_DIR}/ch32v30x_flash.c
#     ${SRC_DIR}/ch32v30x_gpio.c
#     ${SRC_DIR}/ch32v30x_i2c.c
#     ${SRC_DIR}/ch32v30x_it.c
#     ${SRC_DIR}/ch32v30x_iwdg.c
#     ${SRC_DIR}/ch32v30x_misc.c
#     ${SRC_DIR}/ch32v30x_opa.c
#     ${SRC_DIR}/ch32v30x_pwr.c
#     ${SRC_DIR}/ch32v30x_rcc.c
#     ${SRC_DIR}/ch32v30x_rtc.c
#     ${SRC_DIR}/ch32v30x_spi.c
#     ${SRC_DIR}/ch32v30x_tim.c
#     ${SRC_DIR}/ch32v30x_usart.c
#     ${SRC_DIR}/ch32v30x_wwdg.c
#
#     ${SRC_DIR}/core_riscv.c
#     ${SRC_DIR}/system_ch32v30x.c
#     ${SRC_DIR}/debug.c
#
#     # Startup
#     ${INC_DIR}/startup_ch32v30x_D8C.S
# )
#
# add_executable(${PROJECT_NAME}.elf ${SOURCES})
#
# target_include_directories(${PROJECT_NAME}.elf PRIVATE
#     ${INC_DIR}
#     ${SRC_DIR}
# )
#
# # ====================== Линковщик ======================
# set(LINKER_SCRIPT ${SRC_DIR}/Link.ld)
#
# target_link_options(${PROJECT_NAME}.elf PRIVATE
#     -T${LINKER_SCRIPT}
#     -nostartfiles
#     --specs=nosys.specs        # Убрали nano.specs — он не работает в твоём toolchain
#     -mabi=ilp32d
#     -lc
#     -lnosys
#     -Wl,--gc-sections
#     -Wl,--print-memory-usage
# )
#
# # ====================== Пост обработка ======================
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#     COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
#     COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
#     COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
#     COMMENT "Создание .hex / .bin / .lst"
# )
#
# # ====================== Дополнительные команды ======================
#
# add_custom_target(flash
#     COMMAND wlink flash ${PROJECT_NAME}.bin
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Прошивка через wlink..."
#     VERBATIM
# )
#
# add_custom_target(erase
#     COMMAND wlink erase
#     COMMENT "Полное стирание чипа"
#     VERBATIM
# )
# add_custom_target(size
#     COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Размер прошивки"
# )
