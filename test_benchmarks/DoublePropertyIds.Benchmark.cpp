#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include "DoublePropertyIds.h"

constexpr auto totalTodos{1000};

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
