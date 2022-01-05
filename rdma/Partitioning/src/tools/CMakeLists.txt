file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(tools STATIC ${SRC_FILES})

target_include_directories(tools PUBLIC /home/hjkim/rdma/Partitioning/include/tools)


