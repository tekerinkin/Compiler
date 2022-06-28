cmake_minimum_required(VERSION 3.21)
project(Interpreter)

set(CMAKE_CXX_STANDARD 20)

set(INCLUDE
        include/Instruction.h
        include/Interpreter.h)

set(SRC
        src/Interpreter.cpp)

add_library(interpreter_internals
        ${INCLUDE}
        ${SRC})

add_executable(Interpreter src/main.cpp)

target_link_libraries(Interpreter interpreter_internals)
