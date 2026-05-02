file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(scc_ld ${LIB_SOURCES})
add_library(scc::ld ALIAS scc_ld)
target_include_directories(scc_ld PUBLIC include)
target_link_libraries(scc_ld PUBLIC scc::common)

add_executable(sld ${BIN_SOURCES})
target_include_directories(sld PRIVATE include)
target_link_libraries(sld PRIVATE scc::ld)
