#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>

using namespace std::string_literals;
constexpr auto totalTodos{1000};

TodoProperties createProperties(std::string title, std::string description, double timestamp)
{
    return {
            {titleKey, std::move(title)},
            {descriptionKey, std::move(description)},
            {timestampKey, timestamp}
    };
}

TEST_CASE("StringPropertyIds")
{
    StringPropertyIds propertyIds;
    constexpr auto title{"title"};
    for(int i=0; i < totalTodos; i++)
    {
        propertyIds.insert(title, i);
    }

    BENCHMARK("inserting")
                {
                    return propertyIds.insert(title, totalTodos);
                };

    constexpr auto idToBeRemoved{42};
    BENCHMARK("removing")
                {
                    return propertyIds.remove(title, idToBeRemoved);
                };

    BENCHMARK("updating")
                {
                    return propertyIds.updateProperty(title, "newTitle", 0);
                };

    BENCHMARK("querying 1000 todos")
                {
                    return propertyIds.getIds(title);
                };
}

TEST_CASE("DoublePropertyIds")
{
    DoublePropertyIds propertyIds;
    for(int i=0; i < totalTodos; i++)
    {
        propertyIds.insert(i, i);
    }

    constexpr auto idToBeInserted{totalTodos};
    BENCHMARK("inserting")
                {
                    return propertyIds.insert(0, idToBeInserted);
                };

    constexpr auto idToBeRemoved{42};
    BENCHMARK("removing")
                {
                    return propertyIds.remove(0, idToBeRemoved);
                };

    BENCHMARK("updating")
                {
                    return propertyIds.updateProperty(0, 1, 0);
                };

    BENCHMARK("querying 1 todo")
                {
                    return propertyIds.getRangeIds(0, 1);
                };

    BENCHMARK("querying 1000 todo")
                {
                    return propertyIds.getRangeIds(0, totalTodos);
                };
}

ParentStore createDummyStore()
{
    auto timestampInitialValue{0.0};
    ParentStore store;
    for(int i=0; i < totalTodos; i++)
    {
        const auto todoProperties{createProperties("Buy Milk",
                "make of almonds!", timestampInitialValue++)};
        store.insert(i, todoProperties);
    }
    return store;
}

TEST_CASE("Basic store")
{
    constexpr auto id{totalTodos};
    const auto properties{createProperties("Buy Milk",
                                               "make of almonds!", 2392348.12233)};
    ParentStore store{createDummyStore()};

    BENCHMARK("inserting")
                {
                    return store.insert(id, properties);
                };

    const auto propertiesToUpdate{createProperties("Buy Chocolate",
                                           "yummy!", 2392348.12233)};
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
    const auto properties{createProperties("Buy Milk",
                                           "make of almonds!", 1100.12233)};

    ParentStore store;
    store.insert(id, properties);

    const TodoProperty queryProperty{titleKey, "Buy Milk"s};
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

    const TodoProperty queryProperty{titleKey, "Buy Milk"s};
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
    child->update(id, {{titleKey, "Buy Chocolate"s}});

    BENCHMARK("child committing")
                {
                    return child->commit();
                };
}