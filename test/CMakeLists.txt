cmake_minimum_required(VERSION 3.5)

set(test_source_files
        main.test.cpp
        ChildStore.Test.cpp
        ParentStore.Test.cpp
        StringPropertyIds.Test.cpp
        DoublePropertyIds.Test.cpp
        TestUtils
        )

add_executable(test_todo_store
        ${test_source_files}
        )

target_link_libraries(test_todo_store PRIVATE
        todo_store
        catch2
        )

target_include_directories(test_todo_store PRIVATE
        ../src)