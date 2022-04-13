cmake_minimum_required(VERSION 3.16)

project(PiGraph_IPoIB_SSSP)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/network network)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/core core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/query query)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/tools tools)

add_executable (PiGraph_IPoIB_SSSP ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(PiGraph_IPoIB_SSSP core query tools network)
target_compile_features(PiGraph_IPoIB_SSSP PRIVATE cxx_std_17)
add_compile_options(-O3)