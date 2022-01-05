cmake_minimum_required(VERSION 3.16)

project(Partitioning)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools)

add_executable (Partitioning ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(Partitioning tools)
target_compile_features(Partitioning PRIVATE cxx_std_17)