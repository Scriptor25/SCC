file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(scc_ir ${LIB_SOURCES})
add_library(scc::ir ALIAS scc_ir)
target_include_directories(scc_ir PUBLIC include)
target_link_libraries(scc_ir PUBLIC scc::common)

add_executable(sir ${BIN_SOURCES})
target_include_directories(sir PRIVATE include)
target_link_libraries(sir PRIVATE scc::ir)
