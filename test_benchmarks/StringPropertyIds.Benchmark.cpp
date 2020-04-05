#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include "StringPropertyIds.h"

constexpr auto totalTodos{1000};

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

