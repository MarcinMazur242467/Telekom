cmake_minimum_required(VERSION 3.24)
project(MODEM)

set(CMAKE_CXX_STANDARD 17)

add_executable(MODEM sneder.cpp cmake-build-debug/PORTInit.h)
