# To-do Store Example Project
The idea is to create a store to manage to-do entities like this:

```
  struct Todo {
    int64_t id;
    std::string title;
    std::string description;
    double timestamp;
    // and many more...
  };
```
The store provide the following features:
- To-do can be stored.
- Properties from to-do can be retrieved.
- To-do can be removed from the store.
- To-do ids can be queried.
- Child stores can be created in order to provide nested transactions (commit or throw away changes in bulk). 

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
* In order to improve the performance when querying ids by title or by a timestamp range, two property-id associative containers were created. This way when a todo is inserted, updated or removed, the related id is inserted, updated or removed from a id set so is faster to retrieve the ids when querying.
* A benchmark test report can be seen in test_benchmarks/results/last_version.
    * Inserting: 100ns
    * Updating: 189ns
    * Retrieving: 140ns
    * Removing: 50ns
    * Querying a title returning 1 todo: 70ns
    * Querying a title returning 1000 todos: 30us
    * Querying a range returning 1 todo: 86ns
    * Querying a range returning 1000 todos: 82us 
    * Creating child: 24ns
    * Committing child: 129ns
* A new benchmark test report can be created just running the benchmarks test as it is explained in [Running benchmark test](#running-benchmark-tests) section.
* Children have a pointer to the parent, so it can get and query todos from the parent avoiding the need to copy the parent when creating the child.
* When two children commit the same changes to a parent, the parent keep the changes of the last children (so the child's changes that commit first will be lost even if the changes were done after the changes of the other child).
* All examples of using the store can be seen and executed in the test_entity_store target.
* There are still many edge cases for testing and memory accessing protections to be included.
* Children committing in different threads needs to be correctly synchronized (there is no concurrent threading synchronization mechanism right now).