#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>
#include "TestUtils.h"

using namespace std::string_literals;

SCENARIO("Child store")
{
    const auto firstTodoProperties{TestUtils::createProperties("Buy Milk"s,
                                                               "make of almonds!"s,
                                                               2392348.12233)};
    const auto firstTodoPropertiesAfterUpdate{TestUtils::createProperties("Buy Cream"s,
                                                                          "make of almonds!"s,
                                                                          2392348.12233)};

    GIVEN("A store with some todos already inserted")
    {
        auto store{std::make_shared<ParentStore>(TestUtils::createDummyParentStore())};

        WHEN("Creating a child")
        {
            auto child{store->createChild()};

            THEN("Todo properties can be retrieved from child")
            {
                constexpr auto id{0};
                const auto& retrievedProperties{child->get(id)};
                REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, firstTodoProperties));
            }

            WHEN("Updating child properties")
            {
                constexpr auto id{0};
                child->update(id, {{"title", "Buy Cream"s}});

                THEN("Properties from child are updated")
                {
                    const auto& retrievedProperties{child->get(id)};
                    REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, firstTodoPropertiesAfterUpdate));
                }

                THEN("Properties from parent are not updated")
                {
                    const auto& retrievedProperties{store->get(id)};
                    REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, firstTodoProperties));
                }

                AND_WHEN("Child is committed")
                {
                    child->commit();

                    THEN("Properties from parent are updated")
                    {
                        const auto& retrievedProperties{store->get(id)};
                        REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, firstTodoPropertiesAfterUpdate));
                    }
                }
            }

            WHEN("Inserting child todos")
            {
                constexpr auto id{222};
                const auto properties{TestUtils::createProperties("Clean the car"s,
                                                                  "before going to the wedding"s,
                                                                  2392348.12233)};
                child->insert(id, properties);

                THEN("Todo inserted can be retrieved from child")
                {
                    const auto& retrievedProperties{child->get(id)};
                    REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, properties));
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
                        REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, properties));
                    }
                }
            }

            WHEN("Removing todo from child")
            {
                constexpr auto id{0};
                child->remove(id);

                THEN("Todo is removed from child")
                {
                    const auto& retrievedProperties{child->get(id)};
                    REQUIRE(retrievedProperties.empty());
                    REQUIRE_FALSE(child->checkId(id));
                }

                THEN("Todo still exist in parent")
                {
                    const auto& retrievedProperties{store->get(id)};
                    REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, firstTodoProperties));
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
    constexpr auto newTodoId{123};
    const auto newTodoProperties{TestUtils::createProperties("Buy Milk"s,
                                                             "make of almonds!"s,
                                                             1150.12233)};

    GIVEN("A child store with some todos inserted in the parent")
    {
        auto store{std::make_shared<ParentStore>(TestUtils::createDummyParentStore())};
        auto child{store->createChild()};

        THEN("A set of todo ids can be queried from the store")
        {
            TodoProperty queryProperty{titleKey, "Buy Milk"s};
            auto ids{child->query(queryProperty)};
            std::unordered_set<std::int64_t> expectedIds{0, 1};
            REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));

            AND_WHEN("A new todo is inserted")
            {
                constexpr auto id{123};
                const auto properties{TestUtils::createProperties("Buy Milk"s,
                                                                  "make of almonds!"s,
                                                                  1150.12233)};
                child->insert(id, newTodoProperties);

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
                child->insert(newTodoId, newTodoProperties);

                THEN("It is included in the range query")
                {
                    expectedIds = {2, 3, newTodoId};
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
                    ids = child->rangeQuery(minTimeStamp, maxTimeStamp);
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
    constexpr auto newTodoId{123};
    const auto newTodoProperties{TestUtils::createProperties("Buy Milk"s,
                                                             "make of almonds!"s,
                                                             1150.12233)};
    constexpr auto todoIdToUpdate{0};
    const auto propertiesToUpdateFirstChild{TestUtils::createProperties("Buy Cereals"s,
                                                                        "yummy!"s,
                                                                        1150.12233)};
    const auto propertiesToUpdateSecondChild{TestUtils::createProperties("Buy Coke"s,
                                                                         "sugar!"s,
                                                                         1150.12233)};

    GIVEN("A parent store with some todos inserted and two children")
    {
        auto store{std::make_shared<ParentStore>(TestUtils::createDummyParentStore())};
        auto firstChild{store->createChild()};
        auto secondChild{store->createChild()};

        WHEN("A todo is inserted in first child")
        {
            firstChild->insert(newTodoId, newTodoProperties);

            AND_WHEN("The first child is committed")
            {
                firstChild->commit();

                THEN("The todo is present in both the parent and the second child")
                {
                    REQUIRE(store->checkId(newTodoId));
                    REQUIRE(secondChild->checkId(newTodoId));
                }
            }
        }

        WHEN("The first and the second child update same todo")
        {
            firstChild->update(todoIdToUpdate, propertiesToUpdateFirstChild);
            secondChild->update(todoIdToUpdate, propertiesToUpdateSecondChild);

            AND_WHEN("First child is committed")
            {
                firstChild->commit();

                THEN("The second child properties match the ones update by the second child")
                {
                    const auto& retrievedProperties{secondChild->get(todoIdToUpdate)};
                    REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, propertiesToUpdateSecondChild));
                }

                AND_WHEN("Second child is committed")
                {
                    secondChild->commit();

                    THEN("Parent properties match the ones updated by the second child")
                    {
                        const auto& retrievedProperties{store->get(todoIdToUpdate)};
                        REQUIRE(TestUtils::compareTodoProperties(retrievedProperties, propertiesToUpdateSecondChild));
                    }
                }
            }
        }
    }
}
