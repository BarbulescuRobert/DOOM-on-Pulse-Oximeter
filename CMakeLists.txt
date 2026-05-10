cmake_minimum_required(VERSION 3.20)

project(pulseox_doom_oled C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(MCU_FLAGS
    -mcpu=cortex-m0
    -mthumb
)

add_executable(${PROJECT_NAME}.elf
    src/startup_apm32f030.s
    src/system_apm32f030.c
    src/main.c
    src/oled_ssd1306_bitbang.c
    src/doom_frames.c
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    src
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
    -Os
    -g3
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -T${CMAKE_SOURCE_DIR}/linker/APM32F030C8.ld
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -nostartfiles
    --specs=nano.specs
    --specs=nosys.specs
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT "Creating HEX and BIN images"
)
