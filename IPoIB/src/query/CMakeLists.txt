file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)



add_library(query STATIC ${SRC_FILES})

target_include_directories(query PUBLIC ../../include/query)

target_link_libraries(query PRIVATE core)
#target_compile_options(core PRIVATE -Wall -Werror)
