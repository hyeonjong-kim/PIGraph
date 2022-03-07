file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(tools STATIC ${SRC_FILES})

target_include_directories(tools PUBLIC ../../include/tools)

target_link_libraries(tools PRIVATE pthread)

target_compile_features(tools PRIVATE cxx_std_17)

