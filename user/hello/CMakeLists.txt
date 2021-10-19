cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(hello ${SOURCES})

target_include_directories(hello PRIVATE src)

target_link_options(hello PRIVATE "-T${CMAKE_SOURCE_DIR}/misc/user.ld")
