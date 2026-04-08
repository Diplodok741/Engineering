cmake_minimum_required(VERSION 3.20)

# ---- System ----
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ---- Toolchain ----
set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_C_COMPILER_TARGET riscv32)

# clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ch32v20x_firmware C ASM)

set(CMAKE_C_COMPILER_WORKS 1)

# ---- Paths (ABSOLUTE!) ----
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(BASE_DIR ${PROJECT_ROOT}/../..)

set(INC_DIR ${BASE_DIR}/Inc/V20X)
set(SRC_DIR ${BASE_DIR}/Src/V20X)

# ---- Compiler flags ----
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
# )

# ---- Sources ----
set(SOURCES
    ${PROJECT_ROOT}/main.c

    ${SRC_DIR}/ch32v20x_adc.c
    ${SRC_DIR}/ch32v20x_bkp.c
    ${SRC_DIR}/ch32v20x_can.c
    ${SRC_DIR}/ch32v20x_crc.c
    ${SRC_DIR}/ch32v20x_dbgmcu.c
    ${SRC_DIR}/ch32v20x_dma.c
    ${SRC_DIR}/ch32v20x_exti.c
    ${SRC_DIR}/ch32v20x_flash.c
    ${SRC_DIR}/ch32v20x_gpio.c
    ${SRC_DIR}/ch32v20x_i2c.c
    ${SRC_DIR}/ch32v20x_it.c
    ${SRC_DIR}/ch32v20x_iwdg.c
    ${SRC_DIR}/ch32v20x_misc.c
    ${SRC_DIR}/ch32v20x_opa.c
    ${SRC_DIR}/ch32v20x_pwr.c
    ${SRC_DIR}/ch32v20x_rcc.c
    ${SRC_DIR}/ch32v20x_rtc.c
    ${SRC_DIR}/ch32v20x_spi.c
    ${SRC_DIR}/ch32v20x_tim.c
    ${SRC_DIR}/ch32v20x_usart.c
    ${SRC_DIR}/ch32v20x_wwdg.c

    ${SRC_DIR}/core_riscv.c
    ${SRC_DIR}/system_ch32v20x.c
    ${SRC_DIR}/debug.c

    # Startup
    ${INC_DIR}/startup_ch32v20x_D8W.S
)

# ---- Target ----
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# ---- Include dirs (ABSOLUTE!) ----
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${INC_DIR}
    ${SRC_DIR}
)

# ---- Linker ----
set(LINKER_SCRIPT ${SRC_DIR}/Link.ld)

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

# ---- Post build ----
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Generating .hex / .bin / .lst"
)

# ---- Size ----
add_custom_target(size
    COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    DEPENDS ${PROJECT_NAME}.elf
)

# ---- Flash ----
add_custom_target(flash
    COMMAND openocd -f interface/wlink.cfg -f target/ch32v20x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    VERBATIM
)






# cmake_minimum_required(VERSION 3.20)
#
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
#
# set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
# set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
# project(ch32v20x_firmware C ASM)
#
# set(CMAKE_C_COMPILER_WORKS 1)
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
# )
#
# set(SOURCES
#     main.c
#
#     ../../Src/V20X/ch32v20x_adc.c
#     ../../Src/V20X/ch32v20x_bkp.c
#     ../../Src/V20X/ch32v20x_can.c
#     ../../Src/V20X/ch32v20x_crc.c
#     ../../Src/V20X/ch32v20x_dbgmcu.c
#     ../../Src/V20X/ch32v20x_dma.c
#     ../../Src/V20X/ch32v20x_exti.c
#     ../../Src/V20X/ch32v20x_flash.c
#     ../../Src/V20X/ch32v20x_gpio.c
#     ../../Src/V20X/ch32v20x_i2c.c
#     ../../Src/V20X/ch32v20x_it.c
#     ../../Src/V20X/ch32v20x_iwdg.c
#     ../../Src/V20X/ch32v20x_misc.c
#     ../../Src/V20X/ch32v20x_opa.c
#     ../../Src/V20X/ch32v20x_pwr.c
#     ../../Src/V20X/ch32v20x_rcc.c
#     ../../Src/V20X/ch32v20x_rtc.c
#     ../../Src/V20X/ch32v20x_spi.c
#     ../../Src/V20X/ch32v20x_tim.c
#     ../../Src/V20X/ch32v20x_usart.c
#     ../../Src/V20X/ch32v20x_wwdg.c
#
#     ../../Src/V20X/core_riscv.c
#     ../../Src/V20X/system_ch32v20x.c
#     ../../Src/V20X/debug.c
#
#     # Startup
#     ../../Inc/V20X/startup_ch32v20x_D8W.S          # ← измени имя, если у тебя другое
# )
#
# add_executable(${PROJECT_NAME}.elf ${SOURCES})
#
# target_include_directories(${PROJECT_NAME}.elf PRIVATE
#     ../../Inc/V20X
#     ../../Src/V20X
# )
#
# set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/../../Src/V20X/Link.ld")
#
# target_link_options(${PROJECT_NAME}.elf PRIVATE
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
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#     COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
#     COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
#     COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
#     COMMENT "Generating .hex / .bin / .lst"
# )
#
# add_custom_target(size
#     COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
#     DEPENDS ${PROJECT_NAME}.elf
# )
#
# add_custom_target(flash
#     COMMAND openocd -f interface/wlink.cfg -f target/ch32v20x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
#     DEPENDS ${PROJECT_NAME}.elf
#     VERBATIM
# )










# cmake_minimum_required(VERSION 3.20)
#
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
#
# set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
# set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)
#
# project(ch32v203_firmware C ASM)
#
# set(CMAKE_C_COMPILER_WORKS 1)
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
# )
#
# set(SOURCES
#     main.c
#
#     ../../Src/ch32v20x_adc.c
#     ../../Src/ch32v20x_bkp.c
#     ../../Src/ch32v20x_can.c
#     ../../Src/ch32v20x_crc.c
#     ../../Src/ch32v20x_dbgmcu.c
#     ../../Src/ch32v20x_dma.c
#     ../../Src/ch32v20x_exti.c
#     ../../Src/ch32v20x_flash.c
#     ../../Src/ch32v20x_gpio.c
#     ../../Src/ch32v20x_i2c.c
#     ../../Src/ch32v20x_it.c
#     ../../Src/ch32v20x_iwdg.c
#     ../../Src/ch32v20x_misc.c
#     ../../Src/ch32v20x_opa.c
#     ../../Src/ch32v20x_pwr.c
#     ../../Src/ch32v20x_rcc.c
#     ../../Src/ch32v20x_rtc.c
#     ../../Src/ch32v20x_spi.c
#     ../../Src/ch32v20x_tim.c
#     ../../Src/ch32v20x_usart.c
#     ../../Src/ch32v20x_wwdg.c
#
#     ../../Src/core_riscv.c
#     ../../Src/system_ch32v20x.c
#     ../../Src/debug.c
#
#     ../../Inc/startup_ch32v20x_D8W.S
# )
#
# add_executable(${PROJECT_NAME}.elf ${SOURCES})
#
# target_include_directories(${PROJECT_NAME}.elf PRIVATE ../../Inc ../../Src)
#
# set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/../../Src/Link.ld")
#
# target_link_options(${PROJECT_NAME}.elf PRIVATE
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
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#     COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
#     COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
#     COMMAND ${CMAKE_OBJDUMP} -S -d    $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
#     COMMENT "Generating .hex / .bin / .lst"
# )
#
# add_custom_target(size
#     COMMAND riscv32-none-elf-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Firmware size"
# )
#
# add_custom_target(flash
#     COMMAND openocd -f interface/wlink.cfg -f target/ch32v20x.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
#     DEPENDS ${PROJECT_NAME}.elf
#     COMMENT "Flashing firmware"
#     VERBATIM
# )
