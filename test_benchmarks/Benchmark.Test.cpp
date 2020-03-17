#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>

using namespace std::string_literals;

constexpr auto totalTodos{1000};

TEST_CASE("StringPropertyIds")
{
    StringPropertyIds propertyIds;
    for(int i=0; i < totalTodos; i++)
    {
        propertyIds.insert("title", i);
    }

    BENCHMARK("inserting")
                {
                    return propertyIds.insert("title", totalTodos);
                };

    BENCHMARK("updating")
                {
                    return propertyIds.updateProperty("title", "newTitle", 0);
                };

    BENCHMARK("query")
                {
                    return propertyIds.getIds("title");
                };
}

TEST_CASE("DoublePropertyIds")
{
    DoublePropertyIds propertyIds;
    for(int i=0; i < totalTodos; i++)
    {
        propertyIds.insert(i, i);
    }

    BENCHMARK("inserting")
                {
                    return propertyIds.insert(0, totalTodos);
                };

    BENCHMARK("updating")
                {
                    return propertyIds.updateProperty(0, 1, 0);
                };

    BENCHMARK("query")
                {
                    return propertyIds.getRangeIds(0, 1);
                };
}

ParentStore createDummyStore()
{
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
    constexpr auto id{totalTodos};
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
                    return store.update(0, propertiesToUpdate);
                };

    BENCHMARK("retrieving properties")
                {
                    return store.get(0);
                };

    BENCHMARK("removing")
                {
                    return store.remove(0);
                };
}

TEST_CASE("Query small store (contains only one todo)")
{
    constexpr auto id{2133};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 1100.12233}};

    ParentStore store;
    store.insert(id, properties);

    const TodoProperty queryProperty{"title", "Buy Milk"s};
    BENCHMARK("querying small store")
                {
                    return store.query(queryProperty);
                };

    constexpr auto minTimeStamp{1000.0};
    constexpr auto maxTimeStamp{1300.0};
    BENCHMARK("range querying small store")
                {
                    return store.rangeQuery(minTimeStamp, maxTimeStamp);
                };

}

TEST_CASE("Query big store (contains 1000 todos with the same title)")
{
    ParentStore store{createDummyStore()};

    const TodoProperty queryProperty{"title", "Buy Milk"s};
    BENCHMARK("querying")
                {
                    return store.query(queryProperty);
                };

    constexpr auto minTimeStamp{0.0};
    BENCHMARK("range querying")
                {
                    return store.rangeQuery(minTimeStamp, totalTodos);
                };
}

TEST_CASE("Child stores")
{
    auto parent{std::make_shared<ParentStore>(createDummyStore())};
    auto child{parent->createChild()};

    BENCHMARK("creating child")
                {
                    return parent->createChild();
                };

    constexpr auto id{133};
    const TodoProperties propertiesAfterUpdate{{"title", "Buy Milk"s},
                                               {"description", "make of almonds!"s},
                                               {"timestamp", 2392348.12233}};
    child->update(id, {{"title", "Buy Milk"s}});

    BENCHMARK("child committing")
                {
                    return child->commit();
                };
}