# Entity Store Example Project
The instruction given of the example project can be read
[here](project_instructions.md)
.

## Building the project

### Using CMake

To compile any target in the project, you can directly use CMake. Create a build directory manually and generate the 
Makefiles in it. 

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Running tests
```bash
./test/test_entity_store
```

### Running benchmark tests
```bash
./test_benchmarks/test_entity_store_benchmarks
```

## Things to remark and brainstorming
* In order to improve the performance when querying ids by title or by a timestamp range, two property-id associative containers were created. This way when a todo is inserted, updated or removed, the related id is inserted, updated or removed from a id set so is faster to retrieve it when querying.
* A benchmark test report can be seen in test_benchmarks/results/last_version.
    * Inserting: 100ns
    * Updating: 210ns
    * Retrieving: 159ns
    * Removing: 6ns
    * Querying a title returning 1 todo: 70ns
    * Querying a title returning 1000 todos: 30us
    * Querying a range returning 1 todo: 8ns
    * Querying a range returning 1000 todos: 82us 
    * Creating child: 42ns
    * Committing child: 106ns
* Also a new benchmark test report can be created just running the benchmarks test as it is explained in [Running benchmark test](#running-benchmark-tests) section.
* Children have a pointer the the parent, so it can get and query todos from the parent avoiding the need to copy the parent when creating the child.
* When two children commit the same changes to a parent, the parent keep the changes of the last children(so the child changes that commit first will be loose event if the changes were done after the changes of the other child).
* All examples of using the entity store can be seen and executed in the test_entity_store code and executable.
* There are still many edge cases for testing and memory accessing protections to be included.
* Children committing in different threads needs to be correctly synchronized (there is no concurrent threading synchronization mechanism right now).
* In order to improve the range_query C++20 range views features can be used (from rangeV3 or boost libraries). This way a view of the existing id set can be created filtering the elements by a title or a timestamp range. If the client only needs to perform read operations is the fastest way. If the client needs to perform write operations with the id set, a copy would be necessary.