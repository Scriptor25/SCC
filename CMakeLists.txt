cmake_minimum_required(VERSION 3.20)

project(SCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(common)
add_subdirectory(ir)
add_subdirectory(as)
add_subdirectory(cc)
add_subdirectory(ld)

add_library(lib_umbrella INTERFACE)
add_library(scc::umbrella ALIAS lib_umbrella)
target_link_libraries(lib_umbrella INTERFACE scc::common scc::ir scc::as scc::cc scc::ld)
