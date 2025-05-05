cmake_minimum_required(VERSION 3.20)
project(SCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(as)
add_subdirectory(cc)
add_subdirectory(ir)
add_subdirectory(ld)
