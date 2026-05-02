file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(lib_ld ${LIB_SOURCES})
add_library(scc::ld ALIAS lib_ld)
target_include_directories(lib_ld PUBLIC include)
target_link_libraries(lib_ld PUBLIC scc::common)
set_target_properties(lib_ld PROPERTIES OUTPUT_NAME "sld")

add_executable(bin_ld ${BIN_SOURCES})
target_include_directories(bin_ld PRIVATE include)
target_link_libraries(bin_ld PRIVATE scc::ld)
set_target_properties(bin_ld PROPERTIES OUTPUT_NAME "sld")
