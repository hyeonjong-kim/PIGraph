cmake_minimum_required(VERSION 3.16)

project(PiGraph)
add_subdirectory(../lib lib)

add_library(Parsing STATIC ../src/modules/Parsing.cpp)
target_include_directories(Parsing PUBLIC ../include/modules)
target_link_libraries(Parsing PRIVATE utils)

add_compile_options(-O3 -std=c++17)

add_library(Communication STATIC ../src/modules/Communication.cpp)
target_include_directories(Communication PUBLIC ../include/modules)
target_link_libraries(Communication PRIVATE communication pthread)

add_compile_options(-O3 -std=c++17)

add_library(Configuration STATIC ../src/modules/Configuration.cpp)
target_include_directories(Configuration PUBLIC ../include/modules)
target_link_libraries(Configuration PRIVATE zk communication)

#add_compile_options(-O3 -std=c++17)