cmake_minimum_required(VERSION 3.31)
project(Practica)

set(CMAKE_CXX_STANDARD 20)

add_executable(Practica main.cpp)
configure_file(Program.txt Program.txt COPYONLY)
configure_file(Channel.txt Channel.txt COPYONLY)
add_custom_command(TARGET Practica
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/Program.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/Program.txt
)
