file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(lib_ir ${LIB_SOURCES})
add_library(scc::ir ALIAS lib_ir)
target_include_directories(lib_ir PUBLIC include)
target_link_libraries(lib_ir PUBLIC scc::common)
set_target_properties(lib_ir PROPERTIES OUTPUT_NAME "sir")

add_executable(bin_ir ${BIN_SOURCES})
target_include_directories(bin_ir PRIVATE include)
target_link_libraries(bin_ir PRIVATE scc::ir)
set_target_properties(bin_ir PROPERTIES OUTPUT_NAME "sir")

target_compile_options(lib_ir PUBLIC -fsanitize=address)
target_link_libraries(lib_ir PUBLIC asan)
