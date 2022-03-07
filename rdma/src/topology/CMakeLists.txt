file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(topology STATIC ${SRC_FILES})

target_include_directories(topology PUBLIC ../../include/topology)

target_link_libraries(topology PRIVATE core query network tools)
#target_compile_options(topology PRIVATE -fpermissive)
target_compile_features(topology PRIVATE cxx_std_17)