cmake_minimum_required(VERSION 3.14)
project(ReadELF)

set(CMAKE_CXX_STANDARD 20)


include_directories(print_src)
add_library(printLib SHARED print_src/printHello.cpp print_src/printHello.hpp )
add_executable(print print_src/print.cpp)
target_link_libraries(print printLib)


include_directories(src)
add_library(Lib SHARED src/ELF_reader.cpp src/ELF_reader.h)
add_executable(ReadELF src/main.cpp)
target_link_libraries(ReadELF Lib)
