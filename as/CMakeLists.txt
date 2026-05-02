file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(scc_as ${LIB_SOURCES})
add_library(scc::as ALIAS scc_as)
target_include_directories(scc_as PUBLIC include)
target_link_libraries(scc_as PUBLIC scc::common)

add_executable(sas ${BIN_SOURCES})
target_include_directories(sas PRIVATE include)
target_link_libraries(sas PRIVATE scc::as)
