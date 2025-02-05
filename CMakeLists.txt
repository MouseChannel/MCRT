cmake_minimum_required(VERSION 3.22.0)
 
project(MOUSECHANNEL)
set(CMAKE_CXX_STANDARD 20)


set(MCRT_DIR ${CMAKE_SOURCE_DIR}/cmake)
find_package(MCRT REQUIRED)
add_subdirectory(example)
