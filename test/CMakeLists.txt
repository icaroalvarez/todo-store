cmake_minimum_required(VERSION 3.5)

set(test_source_files
        main.test.cpp
        ChildStore.Test.cpp
        ParentStore.Test.cpp
        )

add_executable(test_entity_store
        ${test_source_files}
        )

target_link_libraries(test_entity_store PRIVATE
        entity_store
        CONAN_PKG::Catch2
        )

target_include_directories(test_entity_store PRIVATE
        ../src)