cmake_minimum_required(VERSION 3.21)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compiler main.cpp)

add_subdirectory(Parser)
target_link_libraries(Compiler Parser_internals)

add_subdirectory(Interpreter)
target_link_libraries(Compiler interpreter_internals)
