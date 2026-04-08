cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER   riscv32-none-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-none-elf-gcc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wch_firmware C ASM)

# ---- Выбор чипа ----
set(CHIP "V20X" CACHE STRING "Chip family (V20X or V30X)")

# ---- Пути ----
set(BASE_DIR ${CMAKE_SOURCE_DIR})
set(INC_DIR ${BASE_DIR}/Inc/${CHIP})
set(SRC_DIR ${BASE_DIR}/Src/${CHIP})

# ---- Проект ----
set(PROJECT_DIR ${BASE_DIR}/Projects/${PROJECT_NAME})

# ---- Флаги ----
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

# ---- SOURCES ----
file(GLOB MCU_SOURCES
    ${SRC_DIR}/*.c
)

set(SOURCES
    ${PROJECT_DIR}/main.c
    ${MCU_SOURCES}
    ${SRC_DIR}/core_riscv.c
    ${SRC_DIR}/system_*.c
    ${SRC_DIR}/debug.c
    ${INC_DIR}/startup_*.S
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

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
    --specs=nano.specs
    --specs=nosys.specs
)

# ---- Output ----
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
)
