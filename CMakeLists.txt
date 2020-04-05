cmake_minimum_required(VERSION 3.5)

project(TodoStore)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rd-party)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(test_benchmarks)