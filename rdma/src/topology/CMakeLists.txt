file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(topology STATIC ${SRC_FILES})

target_include_directories(topology PUBLIC ../../include/topology)

target_link_libraries(topology PRIVATE topology)
#target_compile_options(core PRIVATE -Wall -Werror)
