cmake_minimum_required(VERSION 3.16)

project(networktest)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

add_executable (networktest ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(networktest network)
target_compile_features(networktest PRIVATE cxx_std_17)