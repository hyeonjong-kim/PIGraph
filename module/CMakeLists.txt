cmake_minimum_required(VERSION 3.16)

project(PiGraph)
add_subdirectory(../lib lib)

add_library(Parsing STATIC ../src/modules/Parsing.cpp)
target_include_directories(Parsing PUBLIC ../include/modules)
target_link_libraries(Parsing PRIVATE utils)

add_library(Communication STATIC ../src/modules/Communication.cpp)
target_include_directories(Communication PUBLIC ../include/modules)
target_link_libraries(Communication PRIVATE communication pthread)

add_library(Configuration STATIC ../src/modules/Configuration.cpp)
target_include_directories(Configuration PUBLIC ../include/modules)
target_link_libraries(Configuration PRIVATE zk communication utils)

add_library(Coordination STATIC ../src/modules/Coordination.cpp)
target_include_directories(Coordination PUBLIC ../include/modules)
target_link_libraries(Coordination PRIVATE zk utils communication)

add_library(Resource STATIC ../src/modules/Resource.cpp)
target_include_directories(Resource PUBLIC ../include/modules)
target_link_libraries(Resource PRIVATE zk)

add_library(Execution STATIC ../src/modules/Execution.cpp)
target_include_directories(Execution PUBLIC ../include/modules)
target_link_libraries(Execution PRIVATE zk utils)

add_library(Setting STATIC ../src/modules/Setting.cpp)
target_include_directories(Setting PUBLIC ../include/modules)
target_link_libraries(Setting PRIVATE utils)

add_library(IO STATIC ../src/modules/IO.cpp)
target_include_directories(IO PUBLIC ../include/modules)
target_include_directories(IO PUBLIC /home/hjkim/PiGraph/hadoop-2.10.1/include)
target_link_libraries(IO PRIVATE /home/hjkim/PiGraph/hadoop-2.10.1/lib/native/libhdfs.so)

add_library(Graph STATIC ../src/modules/Graph.cpp)
target_include_directories(Graph PUBLIC ../include/modules)
target_link_libraries(Graph PRIVATE struct)

add_library(Network STATIC ../src/modules/Network.cpp)
target_include_directories(Network PUBLIC ../include/modules)
target_link_libraries(Network PRIVATE communication utils)

add_compile_options(-O3 -std=c++17)