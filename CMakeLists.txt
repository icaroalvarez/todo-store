cmake_minimum_required(VERSION 3.5)

project(EntityStore)
set(CMAKE_CXX_STANDARD 17)

include(.conan/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(test_benchmarks)