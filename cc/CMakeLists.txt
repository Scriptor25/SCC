file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)
file(GLOB_RECURSE BIN_SOURCES src/*.cpp)

add_library(scc_cc ${LIB_SOURCES})
add_library(scc::cc ALIAS scc_cc)
target_include_directories(scc_cc PUBLIC include)
target_link_libraries(scc_cc PUBLIC scc::common)

add_executable(scc ${BIN_SOURCES})
target_include_directories(scc PRIVATE include)
target_link_libraries(scc PRIVATE scc::cc)
