# -----------------------------------------------------------------------------
# Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
# The sources are under MIT license.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(tcp_command_server)

file(GLOB_RECURSE SOURCES *.h *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
