cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE CXX_SOURCES "src/*.cpp")

add_library(system STATIC ${CXX_SOURCES})

target_include_directories(system PUBLIC src)
