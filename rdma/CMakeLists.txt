cmake_minimum_required(VERSION 3.16)

project(PiGraph)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/network)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/query)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/topology)


add_executable (PiGraph ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(PiGraph core query tools network toplogy)
target_compile_features(PiGraph PRIVATE cxx_std_17)