cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE CXX_SOURCES "src/*.cpp")

add_library(core STATIC ${CXX_SOURCES})

target_include_directories(core PUBLIC src)
target_link_libraries(core PUBLIC plugs)
