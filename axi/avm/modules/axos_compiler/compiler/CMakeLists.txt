cmake_minimum_required(VERSION 3.10)
project(AxiomCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build the compiler as a SHARED library (.dll/.so) so C# can load it
add_library(axiom SHARED
    lexer.cpp
    AST.cpp
    parser.cpp
    c_api.cpp
)

# Optional: You can still build a standalone executable for testing
add_executable(axiomc main.cpp)
target_link_libraries(axiomc PRIVATE axiom)
