cmake_minimum_required(VERSION 3.15)
project(AxiFOSSEdition VERSION 3.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories for DLLs and EXEs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../components)

# Interfaces (Header-only)
add_library(axi_interfaces INTERFACE)
target_include_directories(axi_interfaces INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/../interfaces>
)

# 1. Axi Filesystem DLL (DAG)
# add_library(axi_filesystem SHARED ../axi_compiler/lib/core/dag_native.c)
# target_link_libraries(axi_filesystem PRIVATE axi_interfaces)

# 2. Axi DVCS DLL
# add_library(axi_dvcs SHARED ../axi_dvcs/main.cpp)
# target_link_libraries(axi_dvcs PRIVATE axi_interfaces)

# 3. Axi Compiler DLL
# add_library(axi_compiler SHARED ../axi_compiler/src/...)
# target_link_libraries(axi_compiler PRIVATE axi_interfaces)

# 4. Axi IDE App
# add_executable(axi_ide ../axi_ide/...)
# target_link_libraries(axi_ide PRIVATE axi_interfaces axi_compiler)

# 5. Axi Compiler Demo App
# add_executable(axi_compiler_demo ../axi_compiler/src/demo_main.cpp)
# target_link_libraries(axi_compiler_demo PRIVATE axi_compiler)
