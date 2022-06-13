cmake_minimum_required(VERSION 3.16)

project(PiGraph_RDMA_SSSP)
#set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/network network)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/core core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/query query)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/tools tools)


add_executable (PiGraph_RDMA_SSSP ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(PiGraph_RDMA_SSSP core query network tools)

target_compile_features(PiGraph_RDMA_SSSP PRIVATE cxx_std_17)