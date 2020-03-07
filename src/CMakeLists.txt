cmake_minimum_required(VERSION 3.5)

set(source_files
        Todo.h
        Store.h
        ParentStore
        ChildStore
        )

add_library(entity_store
        ${source_files}
        )