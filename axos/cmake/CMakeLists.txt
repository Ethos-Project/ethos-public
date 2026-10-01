cmake_minimum_required(VERSION 3.15)
project(Axos VERSION 3.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories for DLLs and EXEs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)

# Interfaces (Header-only)
add_library(axos_interfaces INTERFACE)
target_include_directories(axos_interfaces INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/../interfaces>
)

# 1. Axi Filesystem DLL (DAG) - Axi is the DVCS/DAG language
# add_library(axi_filesystem SHARED ../avm/lib/core/dag_native.c)
# target_link_libraries(axi_filesystem PRIVATE axos_interfaces)

# 2. Axi DVCS DLL
# add_library(axi_dvcs SHARED ../axi/main.cpp)
# target_link_libraries(axi_dvcs PRIVATE axos_interfaces)

# 3. Axos Compiler DLL - Axos is the FOSS execution language
# add_library(axos_compiler SHARED ../avm/modules/axos_compiler/axos_compiler.c)
# target_link_libraries(axos_compiler PRIVATE axos_interfaces)

# 4. Axos IDE App
# add_executable(axos_ide ../axos_ide/...)
# target_link_libraries(axos_ide PRIVATE axos_interfaces axos_compiler)

# 5. Axos AVM / TUI
# add_executable(avm_console ../avm/terminal/skeleton/skeleton_cli.exe.c)
# target_link_libraries(avm_console PRIVATE axos_interfaces axos_compiler)

# 6. Axos Compiler Demo App
# add_executable(axos_compiler_demo ../axos_ide/tests/axi_compiler_demo.cpp)
# target_link_libraries(axos_compiler_demo PRIVATE axos_compiler)
