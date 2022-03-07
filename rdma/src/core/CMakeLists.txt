file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(core STATIC ${SRC_FILES})

target_include_directories(core PUBLIC ../../include/core)

target_link_libraries(core PRIVATE network)

target_compile_features(core PRIVATE cxx_std_17)


