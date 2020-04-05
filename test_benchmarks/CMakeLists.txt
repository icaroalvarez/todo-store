cmake_minimum_required(VERSION 3.5)

set(test_source_files
        main.test.cpp
        Store.Benchmark.cpp
        DoublePropertyIds.Benchmark.cpp
        StringPropertyIds.Benchmark.cpp
        )

add_executable(test_todo_store_benchmarks
        ${test_source_files}
        )

target_link_libraries(test_todo_store_benchmarks PRIVATE
        todo_store
        catch2
        )

target_include_directories(test_todo_store_benchmarks PRIVATE
        ../src)