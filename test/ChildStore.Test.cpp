#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>

using namespace std::string_literals;

namespace
{
    bool compareTodoProperties(const TodoProperties &lhs, const TodoProperties &rhs)
    {
        const auto sameTitle{std::get<std::string>(lhs.at("title")) ==
                             std::get<std::string>(rhs.at("title"))};
        const auto sameDescription{std::get<std::string>(lhs.at("description")) ==
                                   std::get<std::string>(rhs.at("description"))};
        const auto sameTimestamp{std::get<double>(lhs.at("timestamp")) ==
                                 std::get<double>(rhs.at("timestamp"))};
        return sameTitle && sameDescription && sameTimestamp;
    }

    ParentStore createDummyStore()
    {
        ParentStore store;
        auto id{0};
        store.insert(id++, {{"title",       "Buy Milk"s},
                            {"description", "make of almonds!"s},
                            {"timestamp",   2392348.12233}});
        store.insert(id++, {{"title",       "Buy Milk"s},
                            {"description", "don't forget!"s},
                            {"timestamp",   2400050.12555}});
        store.insert(id++, {{"title",       "Study Chinese"s},
                            {"description", "worth it!"s},
                            {"timestamp",   1000.0}});
        store.insert(id, {{"title",       "Call mom"s},
                          {"description", "is her birthday"s},
                          {"timestamp",   1200.0}});
        return store;
    }

    SCENARIO("Child stores")
    {
        GIVEN("A store with some properties")
        {
            auto store{std::make_shared<ParentStore>(createDummyStore())};

            WHEN("Creating a child")
            {
                auto child{store->createChild()};

                THEN("Todo properties can be retrieved from child")
                {
                    constexpr auto id{0};
                    const auto& retrievedProperties{child->get(id)};

                    const TodoProperties expectedProperties{{"title",       "Buy Milk"s},
                                                            {"description", "make of almonds!"s},
                                                            {"timestamp",   2392348.12233}};
                    REQUIRE(compareTodoProperties(retrievedProperties, expectedProperties));
                }

                WHEN("Updating child properties")
                {
                    constexpr auto id{0};
                    child->update(id, {{"title", "Buy Cream"s}});

                    THEN("Properties from parent were updated")
                    {
                        const TodoProperties propertiesAfterUpdate{{"title",       "Buy Cream"s},
                                                                   {"description", "make of almonds!"s},
                                                                   {"timestamp",   2392348.12233}};
                        const auto& retrievedProperties{child->get(id)};
                        REQUIRE(compareTodoProperties(retrievedProperties, propertiesAfterUpdate));
                    }

                    THEN("Properties from parent were not updated")
                    {
                        const TodoProperties expectedProperties{{"title",       "Buy Milk"s},
                                                                {"description", "make of almonds!"s},
                                                                {"timestamp",   2392348.12233}};
                        const auto& retrievedProperties{store->get(id)};
                        REQUIRE(compareTodoProperties(retrievedProperties, expectedProperties));
                    }

                    AND_WHEN("Child is committed")
                    {
                        child->commit();

                        THEN("Properties from parent were updated")
                        {
                            const TodoProperties propertiesAfterUpdate{{"title",       "Buy Cream"s},
                                                                       {"description", "make of almonds!"s},
                                                                       {"timestamp",   2392348.12233}};
                            const auto& retrievedProperties{store->get(id)};
                            REQUIRE(compareTodoProperties(retrievedProperties, propertiesAfterUpdate));
                        }
                    }
                }

                WHEN("Inserting child todos")
                {
                    constexpr auto id{222};
                    const TodoProperties properties{{"title",       "Clean the car"s},
                                                    {"description", "before going to the wedding"s},
                                                    {"timestamp",   2392348.12233}};
                    child->insert(id, properties);

                    THEN("Todo inserted can be retrieved from child")
                    {
                        const auto& retrievedProperties{child->get(id)};
                        REQUIRE(compareTodoProperties(retrievedProperties, properties));
                    }

                    THEN("Todo inserted in the child cannot be retrieved from parent")
                    {
                        REQUIRE_FALSE(store->checkId(id));
                    }

                    AND_WHEN("Child is committed")
                    {
                        child->commit();

                        THEN("Inserted Todo can be retrieved from parent")
                        {
                            const auto& retrievedProperties{store->get(id)};
                            REQUIRE(compareTodoProperties(retrievedProperties, properties));
                        }
                    }
                }

                WHEN("Removing todo from child")
                {
                    constexpr auto id{3};
                    child->remove(id);

                    THEN("Todo is removed from child")
                    {
                        const auto& retrievedProperties{child->get(id)};
                        REQUIRE(retrievedProperties.empty());
                        REQUIRE_FALSE(child->checkId(id));
                    }

                    THEN("Todo still exist in parent")
                    {
                        const TodoProperties properties{{"title",       "Call mom"s},
                                                        {"description", "is her birthday"s},
                                                        {"timestamp",   1200.0}};
                        const auto& retrievedProperties{store->get(id)};
                        REQUIRE(compareTodoProperties(retrievedProperties, properties));
                        REQUIRE(store->checkId(id));
                    }

                    AND_WHEN("Child is committed")
                    {
                        child->commit();
                        THEN("Todo is removed from parent")
                        {
                            const auto& retrievedProperties{child->get(id)};
                            REQUIRE(retrievedProperties.empty());
                            REQUIRE_FALSE(store->checkId(id));
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Child store queries")
    {
        GIVEN("A child store with some todos")
        {
            auto store{std::make_shared<ParentStore>(createDummyStore())};
            auto child{store->createChild()};

            THEN("A set of todo ids can be queried from the store")
            {
                TodoProperty queryProperty{"title", "Buy Milk"s};
                auto ids{child->query(queryProperty)};
                std::unordered_set<std::int64_t> expectedIds{0, 1};
                REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));

                AND_WHEN("A new todo is inserted")
                {
                    constexpr auto id{123};
                    const TodoProperties properties{{"title",       "Buy Milk"s},
                                                    {"description", "make of almonds!"s},
                                                    {"timestamp",   1150.12233}};
                    child->insert(id, properties);

                    THEN("The new id is returned when querying")
                    {
                        ids = child->query(queryProperty);
                        expectedIds = {0, 1, id};
                        REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));
                    }
                }

                AND_WHEN("A todo title is updated")
                {
                    constexpr auto idToUpdate{1};
                    const TodoProperties propertiesToUpdate{{"title", "Buy Cereals"s}};
                    child->update(idToUpdate, propertiesToUpdate);

                    THEN("The query result do not include the updated todo id")
                    {
                        ids = child->query(queryProperty);
                        expectedIds = {0};
                        REQUIRE(ids == expectedIds);
                    }

                    THEN("The updated todo can be retrieved when querying with the updated title")
                    {
                        queryProperty = {"title", "Buy Cereals"s};
                        ids = child->query(queryProperty);
                        expectedIds = {1};
                        REQUIRE(ids == expectedIds);
                    }
                }

                AND_WHEN("An id is removed")
                {
                    constexpr auto idToBeRemoved{1};
                    child->remove(idToBeRemoved);

                    THEN("The query result do not include the updated todo id")
                    {
                        ids = child->query(queryProperty);
                        expectedIds = {0};
                        REQUIRE(ids == expectedIds);
                    }
                }
            }

            THEN("A set of todo ids can be queried from the store using a timestamp range")
            {
                auto minTimeStamp{1000.0};
                auto maxTimeStamp{1300.0};
                auto ids{child->rangeQuery(minTimeStamp, maxTimeStamp)};
                std::unordered_set<std::int64_t> expectedIds{2, 3};
                REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));

                AND_WHEN("A new todo is inserted")
                {
                    constexpr auto id{123};
                    const TodoProperties properties{{"title",       "Buy Milk"s},
                                                    {"description", "make of almonds!"s},
                                                    {"timestamp",   1150.12233}};
                    child->insert(id, properties);

                    THEN("It is included in the range query")
                    {
                        expectedIds = {2, 3, id};
                        ids = child->rangeQuery(minTimeStamp, maxTimeStamp);
                        REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));
                    }
                }

                AND_WHEN("A timestamp is updated")
                {
                    constexpr auto idToUpdate{3};
                    const TodoProperties propertiesToUpdate{{"timestamp", 1800}};
                    child->update(idToUpdate, propertiesToUpdate);

                    THEN("The query do not include the updated todo id")
                    {
                        auto ids = child->rangeQuery(minTimeStamp, maxTimeStamp);
                        expectedIds = {2};
                        REQUIRE(ids == expectedIds);
                    }

                    THEN("Range query including the new timestamp includes the updated id")
                    {
                        minTimeStamp = 1500;
                        maxTimeStamp = 2000;
                        ids = child->rangeQuery(minTimeStamp, maxTimeStamp);
                        expectedIds = {3};
                        REQUIRE(ids == expectedIds);
                    }
                }

                AND_WHEN("An id is removed")
                {
                    child->remove(2);

                    THEN("The range query does not include that id")
                    {
                        expectedIds = {3};
                        ids = child->rangeQuery(minTimeStamp, maxTimeStamp);
                        REQUIRE(ids == expectedIds);
                    }
                }
            }
        }
    }

    SCENARIO("Multiple children")
    {
        GIVEN("A parent store with some todos inserted and two children")
        {
            auto store{std::make_shared<ParentStore>(createDummyStore())};
            auto firstChild{store->createChild()};
            auto secondChild{store->createChild()};

            WHEN("A todo is inserted in first child")
            {
                constexpr auto id{123};
                const TodoProperties properties{{"title",       "Buy Milk"s},
                                                {"description", "make of almonds!"s},
                                                {"timestamp",   1150.12233}};
                firstChild->insert(id, properties);

                AND_WHEN("The first child is committed")
                {
                    firstChild->commit();

                    THEN("The todo is present in both the parent and the second child")
                    {
                        REQUIRE(store->checkId(id));
                        REQUIRE(secondChild->checkId(id));
                    }
                }
            }

            WHEN("The first and the second child update same todo")
            {
                constexpr auto id{0};
                const TodoProperties propertiesFirstChild{{"title",       "Buy Cereals"s},
                                                          {"description", "yummy!"s},
                                                          {"timestamp",   1150.12233}};
                firstChild->update(id, propertiesFirstChild);

                const TodoProperties propertiesSecondChild{{"title",       "Buy Coke"s},
                                                           {"description", "sugar!"s},
                                                           {"timestamp",   1150.12233}};
                secondChild->update(id, propertiesSecondChild);

                AND_WHEN("First child is committed")
                {
                    firstChild->commit();

                    THEN("The second child properties match the ones update by the second child")
                    {
                        const auto& retrievedProperties{secondChild->get(id)};
                        REQUIRE(compareTodoProperties(retrievedProperties, propertiesSecondChild));
                    }

                    AND_WHEN("Second child is committed")
                    {
                        secondChild->commit();

                        THEN("Parent properties match the ones updated by the second child")
                        {
                            const auto& retrievedProperties{store->get(id)};
                            REQUIRE(compareTodoProperties(retrievedProperties, propertiesSecondChild));
                        }
                    }
                }
            }
        }
    }
}
