cmake_minimum_required(VERSION 3.16)
add_subdirectory(../lib lib)

project(PiGraph)

add_executable (master main.cpp)
target_link_libraries(master PRIVATE HDFS)

