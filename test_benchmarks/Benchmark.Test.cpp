#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <Store.h>

using namespace std::string_literals;

TEST_CASE("Basic store")
{
    constexpr auto id{2133};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    Store store;

    BENCHMARK("inserting")
                {
                    return store.insert(id, properties);
                };

    const TodoProperties propertiesToUpdate{{"title",       "Buy Milk"s},
                                            {"description", "make of almonds!"s},
                                            {"timestamp",   2392348.12233}};
    BENCHMARK("updating")
                {
                    return store.update(id, propertiesToUpdate);
                };

    BENCHMARK("retrieving properties")
                {
                    return store.get(id);
                };

    BENCHMARK("removing")
                {
                    return store.remove(id);
                };
}

Store createDummyStore()
{
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    constexpr auto totalTodos{1000*1000};
    Store store;
    for(int i=0; i < totalTodos; i++)
    {
        store.insert(i, properties);
    }
    return store;
}

TEST_CASE("Store queries")
{
    Store store{createDummyStore()};
    
    const TodoProperty queryProperty{"title", "Buy Milk"s};
    BENCHMARK("querying")
                {
                    return store.query(queryProperty);
                };

    constexpr auto minTimeStamp{1000.0};
    constexpr auto maxTimeStamp{1300.0};
    BENCHMARK("querying")
                {
                    return store.rangeQuery(minTimeStamp, maxTimeStamp);
                };
}
