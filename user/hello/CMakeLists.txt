cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE CXX_SOURCES "src/*.cpp")

add_executable(hello ${CXX_SOURCES})

target_include_directories(hello PRIVATE src)
target_link_libraries(hello PRIVATE core system)

target_link_options(hello PRIVATE "-T${CMAKE_SOURCE_DIR}/misc/user.ld")
