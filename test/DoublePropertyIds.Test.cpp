#include <catch2/catch.hpp>
#include "DoublePropertyIds.h"

using namespace std::string_literals;

SCENARIO("Collect ids from double properties")
{
    GIVEN("A double property id container")
    {
        DoublePropertyIds doublePropertyIds;

        WHEN("Three double properties and ids are inserted")
        {
            auto propertyValue{100.0};
            auto id{0};
            doublePropertyIds.insert(propertyValue, id++);
            propertyValue = 200.0;
            doublePropertyIds.insert(propertyValue, id++);
            propertyValue = 300.0;
            doublePropertyIds.insert(propertyValue, id++);

            THEN("A set of ids can be retrieved giving the a property range value")
            {
                const std::unordered_set<std::int64_t> expectedIds{0, 1};
                const auto minValue{100.0};
                const auto maxValue{200.0};
                const auto ids{doublePropertyIds.getRangeIds(minValue, maxValue)};
                REQUIRE(ids == expectedIds);
            }

            AND_WHEN("A property is updated")
            {
                constexpr auto idToUpdate{0};
                const auto oldPropertyValue{100.0};
                const auto newPropertyValue{50.0};
                doublePropertyIds.updateProperty(oldPropertyValue, newPropertyValue, idToUpdate);

                THEN("Retrieving ids from range does not include the id updated")
                {
                    const std::unordered_set<std::int64_t> expectedIds{1};
                    const auto minValue{100.0};
                    const auto maxValue{200.0};
                    const auto ids{doublePropertyIds.getRangeIds(minValue, maxValue)};
                    REQUIRE(ids == expectedIds);
                }
            }

            AND_WHEN("A double property is removed")
            {
                id = 0;
                propertyValue = 100.0;
                doublePropertyIds.remove(propertyValue, id);

                THEN("The id is not included when retrieving the ids of a range property value")
                {
                    const std::unordered_set<std::int64_t> expectedIds{1};
                    const auto minValue{100.0};
                    const auto maxValue{200.0};
                    const auto ids{doublePropertyIds.getRangeIds(minValue, maxValue)};
                    REQUIRE(ids == expectedIds);
                }
            }
        }
    }
}
