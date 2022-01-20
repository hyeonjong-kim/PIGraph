cmake_minimum_required(VERSION 3.16)

project(PiGraph_IPoIB)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/query)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

add_executable (PiGraph_IPoIB ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(PiGraph_IPoIB core query tools network)
target_compile_features(PiGraph_IPoIB PRIVATE cxx_std_17)