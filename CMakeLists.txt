﻿# CMakeList.txt : CMake project for cppjs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("jcc")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 17)

# Add source to this project's executable.
add_executable (cppjs "jcc_test.cpp")

# TODO: Add tests and install targets if needed.
