file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(cclib STATIC ${lib-sources})
target_include_directories(cclib PUBLIC include)

add_executable(cc ${bin-sources})
target_include_directories(cc PRIVATE include)

target_link_libraries(cc PRIVATE cclib)
