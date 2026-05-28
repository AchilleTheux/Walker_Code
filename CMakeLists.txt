# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(walker C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
				
# Add executable. Default name is the project name, version 0.1

add_executable(walker

main.c

Feetech/STS_Uart_Half_Duplex.c
Feetech/STS.c
Feetech/STS_Actions.c
Feetech/Registers_Feetech.c

user.c
Interpreteur.c
Walker_Kinematics.c
)
 
# Feetech half-duplex UART PIO sources.
pico_generate_pio_header(walker ${CMAKE_CURRENT_LIST_DIR}/Feetech/STS_pepino_uart_tx.pio)
pico_generate_pio_header(walker ${CMAKE_CURRENT_LIST_DIR}/Feetech/STS_uart_rx.pio)


pico_set_program_name(walker "walker")
pico_set_program_version(walker "0.1")


# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_usb(walker 1)

pico_enable_stdio_uart(walker 1)
#carte rp2040_zero, projet leds 2025 : uart sur pins 12 et 13
target_compile_definitions(walker PRIVATE
    PICO_DEFAULT_UART_TX_PIN=0
    PICO_DEFAULT_UART_RX_PIN=1
)

# Add the standard include files to the build
target_include_directories(walker PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(walker
        pico_stdlib
        hardware_pio
        m
        )
        
set(PICO_32BIT 1)
pico_add_extra_outputs(walker)
