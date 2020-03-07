#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>

using namespace std::string_literals;

ParentStore createDummyStore()
{
    constexpr auto totalTodos{1000*1000};
    auto timestampInitialValue{0.0};
    ParentStore store;
    for(int i=0; i < totalTodos; i++)
    {
        store.insert(i, {{"title", "Buy Milk"s},
                         {"description", "make of almonds!"s},
                         {"timestamp", timestampInitialValue++}});
    }
    return store;
}

TEST_CASE("Basic store")
{
    constexpr auto id{1000*1000};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    ParentStore store{createDummyStore()};

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



TEST_CASE("Store queries")
{
    ParentStore store{createDummyStore()};

    const TodoProperty queryProperty{"title", "Buy Milk"s};
    BENCHMARK("querying")
                {
                    return store.query(queryProperty);
                };

    constexpr auto minTimeStamp{1000.0};
    constexpr auto maxTimeStamp{1300.0};
    BENCHMARK("range querying")
                {
                    return store.rangeQuery(minTimeStamp, maxTimeStamp);
                };
}

TEST_CASE("Child stores")
{
    ParentStore parentStore{createDummyStore()};
    auto parent{std::make_shared<ParentStore>(parentStore)};
    auto child{parent->createChild()};

    BENCHMARK("creating child")
                {
                    return parent->createChild();
                };

    constexpr auto id{2133};
    const TodoProperties propertiesAfterUpdate{{"title", "Buy Milk"s},
                                               {"description", "make of almonds!"s},
                                               {"timestamp", 2392348.12233}};
    child->update(id, {{"title", "Buy Milk"s}});

    BENCHMARK("child committing")
                {
                    return child->commit();
                };
}