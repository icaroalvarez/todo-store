cmake_minimum_required(VERSION 3.5)

set(source_files
        Todo.h
        Store.h
        ParentStore
        ChildStore
        StringPropertyIds
        DoublePropertyIds
        )

add_library(todo_store
        ${source_files}
        )