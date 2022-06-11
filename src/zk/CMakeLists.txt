add_library(zk STATIC ../src/zk/zkTool.cpp)
target_include_directories(../include/zk)
target_link_libraries(zk STATIC zookeeper_mt)