file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(lib_cc ${LIB_SOURCES})
add_library(scc::cc ALIAS lib_cc)
target_include_directories(lib_cc PUBLIC include)
target_link_libraries(lib_cc PUBLIC scc::common)
set_target_properties(lib_cc PROPERTIES OUTPUT_NAME "scc")

add_executable(bin_cc ${BIN_SOURCES})
target_include_directories(bin_cc PRIVATE include)
target_link_libraries(bin_cc PRIVATE scc::cc)
set_target_properties(bin_cc PROPERTIES OUTPUT_NAME "scc")
