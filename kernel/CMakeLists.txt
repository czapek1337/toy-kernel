cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE CXX_SOURCES "src/*.cpp")
file(GLOB_RECURSE ASM_SOURCES "src/*.asm")

add_executable(kernel ${CXX_SOURCES} ${ASM_SOURCES})

target_include_directories(kernel PRIVATE src)
target_link_libraries(kernel PRIVATE core plugs system)

target_link_options(kernel PRIVATE "-T${CMAKE_SOURCE_DIR}/misc/kernel.ld")
