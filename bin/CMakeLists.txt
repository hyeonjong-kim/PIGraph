cmake_minimum_required(VERSION 3.16)
add_subdirectory(../module module)

project(PiGraph)

add_executable (master master_main.cpp)
target_link_libraries(master PRIVATE Communication Configuration Coordination pthread)

add_executable (client client_main.cpp)
target_link_libraries(client PRIVATE Parsing Communication)

add_executable (wm wm_main.cpp)
target_link_libraries(wm PRIVATE Resource Execution pthread)
add_compile_options(-O3 -std=c++17)


add_executable (worker worker_main.cpp)
target_link_libraries(wm PRIVATE Resource Execution pthread)
add_compile_options(-O3 -std=c++17)
