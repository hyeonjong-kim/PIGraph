cmake_minimum_required(VERSION 3.16)
project(PiGraph)

file(GLOB_RECURSE SRC_FILES1 CONFIGURE_DEPENDS
  ../src/utils/*.cpp
)

add_library(utils STATIC ${SRC_FILES1})
target_include_directories(utils PUBLIC ../include/utils)

add_compile_options(-O3 -std=c++17)

file(GLOB_RECURSE SRC_FILES2 CONFIGURE_DEPENDS
  ../src/zk/*.cpp
)

add_library(zk STATIC ${SRC_FILES2})
target_include_directories(zk PUBLIC ../include/zk)
target_link_libraries(zk PRIVATE zookeeper_mt)

add_compile_options(-O3 -std=c++17)

file(GLOB_RECURSE SRC_FILES3 CONFIGURE_DEPENDS
  ../src/communication/*.cpp
)

add_library(communication STATIC ${SRC_FILES3})
target_include_directories(communication PUBLIC ../include/communication)
target_link_libraries(communication PRIVATE pthread ibverbs)
add_compile_options(-O3 -std=c++17)

file(GLOB_RECURSE SRC_FILES4 CONFIGURE_DEPENDS
  ../src/struct/*.cpp
)

add_library(struct STATIC ${SRC_FILES4})
target_include_directories(struct PUBLIC ../include/struct)

add_compile_options(-O3 -std=c++17)


#file(GLOB_RECURSE SRC_FILES4 CONFIGURE_DEPENDS
#  ../src/io/*.cpp
#)

#add_library(HDFS STATIC ${SRC_FILES4})
#target_include_directories(HDFS PUBLIC ../include/io)
#target_include_directories(HDFS PUBLIC /home/hjkim/PiGraph/hadoop-2.10.1/include)
#target_link_libraries(HDFS PRIVATE /home/hjkim/PiGraph/hadoop-2.10.1/lib/native/libhdfs.so)

#add_compile_options(-O3 -std=c++17)