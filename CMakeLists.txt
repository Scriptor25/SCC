cmake_minimum_required(VERSION 3.20)
project(SCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE as-src as/lib/src/*.cpp as/include/*.hpp)
file(GLOB_RECURSE cc-src cc/lib/src/*.cpp cc/include/*.hpp)
file(GLOB_RECURSE ir-src ir/lib/src/*.cpp ir/include/*.hpp)
file(GLOB_RECURSE ld-src ld/lib/src/*.cpp ld/include/*.hpp)
file(GLOB_RECURSE sas-src as/src/*.cpp as/include/*.hpp)
file(GLOB_RECURSE scc-src cc/src/*.cpp cc/include/*.hpp)
file(GLOB_RECURSE sir-src ir/src/*.cpp ir/include/*.hpp)
file(GLOB_RECURSE sld-src ld/src/*.cpp ld/include/*.hpp)

add_library(as STATIC ${as-src})
target_include_directories(as PUBLIC as/include)

add_library(cc STATIC ${cc-src})
target_include_directories(cc PUBLIC cc/include)

add_library(ir STATIC ${ir-src})
target_include_directories(ir PUBLIC ir/include)

add_library(ld STATIC ${ld-src})
target_include_directories(ld PUBLIC ld/include)

add_executable(sas ${sas-src})
target_include_directories(sas PRIVATE as/include)
target_link_libraries(sas PRIVATE as)

add_executable(scc ${scc-src})
target_include_directories(scc PRIVATE cc/include)
target_link_libraries(scc PRIVATE cc)

add_executable(sir ${sir-src})
target_include_directories(sir PRIVATE ir/include)
target_link_libraries(sir PRIVATE ir)

add_executable(sld ${sld-src})
target_include_directories(sld PRIVATE ld/include)
target_link_libraries(sld PRIVATE ld)
