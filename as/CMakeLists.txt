file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(lib_as ${LIB_SOURCES})
add_library(scc::as ALIAS lib_as)
target_include_directories(lib_as PUBLIC include)
target_link_libraries(lib_as PUBLIC scc::common)
set_target_properties(lib_as PROPERTIES OUTPUT_NAME "sas")

add_executable(bin_as ${BIN_SOURCES})
target_include_directories(bin_as PRIVATE include)
target_link_libraries(bin_as PRIVATE scc::as)
set_target_properties(bin_as PROPERTIES OUTPUT_NAME "sas")
