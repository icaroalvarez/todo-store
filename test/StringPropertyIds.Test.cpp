#include <catch2/catch.hpp>
#include "StringPropertyIds.h"

using namespace std::string_literals;

SCENARIO("Collect ids from string properties")
{
    GIVEN("A string property id container")
    {
        StringPropertyIds stringPropertyIds;

        WHEN("Three string properties and ids are inserted")
        {
            auto propertyValue{"Buy milk"s};
            auto id{0};
            stringPropertyIds.insert(propertyValue, id++);
            stringPropertyIds.insert(propertyValue, id++);
            propertyValue = {"Call mom"s};
            stringPropertyIds.insert(propertyValue, id++);

            THEN("A set of ids can be retrieved giving the property")
            {
                const std::unordered_set<std::int64_t> expectedIds{0, 1};
                const auto ids{stringPropertyIds.getIds("Buy milk"s)};
                REQUIRE(ids == expectedIds);
            }

            AND_WHEN("A string property is updated")
            {
                constexpr auto idToUpdate{0};
                const auto oldPropertyValue{"Buy milk"s};
                const auto newPropertyValue{"Clean the car"s};
                stringPropertyIds.updateProperty(oldPropertyValue, newPropertyValue, idToUpdate);

                THEN("The set of ids retrieved with the old property does not contain the id of the property updated")
                {
                    const std::unordered_set<std::int64_t> expectedIds{1};
                    const auto ids{stringPropertyIds.getIds(oldPropertyValue)};
                    REQUIRE(ids == expectedIds);
                }

                THEN("A set of ids can be retrieved giving the new property")
                {
                    const std::unordered_set<std::int64_t> expectedIds{0};
                    const auto ids{stringPropertyIds.getIds(newPropertyValue)};
                    REQUIRE(ids == expectedIds);
                }
            }

            AND_WHEN("A string property is removed")
            {
                id = 0;
                propertyValue = {"Buy milk"s};
                stringPropertyIds.remove(propertyValue, id);

                THEN("The id is not included when retrieving the ids of that property value")
                {
                    const std::unordered_set<std::int64_t> expectedIds{1};
                    const auto ids{stringPropertyIds.getIds(propertyValue)};
                    REQUIRE(ids == expectedIds);
                }
            }
        }
    }
}
