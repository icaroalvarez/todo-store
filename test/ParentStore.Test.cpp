#include <catch2/catch.hpp>
#include <Store.h>
#include <ParentStore.h>

using namespace std::string_literals;

bool compareTodoProperties(const TodoProperties& lhs, const TodoProperties& rhs)
{
    const auto sameTitle{std::get<std::string>(lhs.at("title")) ==
                         std::get<std::string>(rhs.at("title"))};
    const auto sameDescription{std::get<std::string>(lhs.at("description")) ==
                               std::get<std::string>(rhs.at("description"))};
    const auto sameTimestamp{std::get<double>(lhs.at("timestamp")) ==
                             std::get<double>(rhs.at("timestamp"))};
    return sameTitle && sameDescription && sameTimestamp;
}

SCENARIO("Basic store")
{
    GIVEN("An empty store")
    {
        ParentStore store;

        WHEN("A todo is inserted")
        {
            constexpr auto id{2133};
            const TodoProperties properties{{"title",       "Buy Milk"s},
                                            {"description", "make of almonds!"s},
                                            {"timestamp",   2392348.12233}};
            store.insert(id, properties);

            THEN("The properties of the todo id can be retrieved")
            {
                const auto& retrievedProperties{store.get(id)};
                REQUIRE(compareTodoProperties(retrievedProperties, properties));
            }

            THEN("The todo id properties can be updated")
            {
                const TodoProperties propertiesToUpdate{{"title",       "Buy Milk"s},
                                                        {"description", "make of almonds!"s},
                                                        {"timestamp",   2392348.12233}};
                store.update(id, propertiesToUpdate);
                const auto& retrievedProperties{store.get(id)};
                REQUIRE(compareTodoProperties(retrievedProperties, propertiesToUpdate));
            }


            WHEN("The todo can be removed from the store")
            {
                store.remove(id);
                const auto& idExitsInStore{store.checkId(id)};
                REQUIRE_FALSE(idExitsInStore);
            }

        }
    }
};

ParentStore createDummyStore()
{
    ParentStore store;
    auto id{0};
    store.insert(id++, {{"title", "Buy Milk"s},
                        {"description", "make of almonds!"s},
                        {"timestamp", 2392348.12233}});
    store.insert(id++, {{"title", "Buy Milk"s},
                        {"description", "don't forget!"s},
                        {"timestamp", 2400050.12555}});
    store.insert(id++, {{"title", "Study Chinese"s},
                        {"description", "worth it!"s},
                        {"timestamp", 1000.0}});
    store.insert(id, {{"title", "Call mom"s},
                      {"description", "is her birthday"s},
                      {"timestamp", 1200.0}});
    return store;
}

SCENARIO("Store queries")
{
    GIVEN("A store with some todos")
    {
        ParentStore store{createDummyStore()};

        THEN("A set of todo ids can be queried from the store")
        {
            TodoProperty queryProperty{"title", "Buy Milk"s};
            auto ids{store.query(queryProperty)};
            std::unordered_set<std::int64_t> expectedIds{0, 1};
            REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));

            AND_WHEN("A todo title is updated")
            {
                constexpr auto idToUpdate{1};
                const TodoProperties propertiesToUpdate{{"title", "Buy Cereals"s}};
                store.update(idToUpdate, propertiesToUpdate);

                THEN("The query result do not include the updated todo id")
                {
                    ids = store.query(queryProperty);
                    expectedIds = {0};
                    REQUIRE(ids == expectedIds);
                }

                THEN("The updated todo can be retrieved when querying with the updated title")
                {
                    queryProperty = {"title", "Buy Cereals"s};
                    ids = store.query(queryProperty);
                    expectedIds = {1};
                    REQUIRE(ids == expectedIds);
                }
            }

            AND_WHEN("An id is removed")
            {
                constexpr auto idToBeRemoved{1};
                store.remove(idToBeRemoved);

                THEN("The query result do not include the updated todo id")
                {
                    ids = store.query(queryProperty);
                    expectedIds = {0};
                    REQUIRE(ids == expectedIds);
                }
            }
        }

        THEN("A set of todo ids can be queried from the store using a timestamp range")
        {
            auto minTimeStamp{1000.0};
            auto maxTimeStamp{1300.0};
            auto ids{store.rangeQuery(minTimeStamp, maxTimeStamp)};
            std::unordered_set<std::int64_t> expectedIds{2, 3};
            REQUIRE(std::is_permutation(expectedIds.cbegin(), expectedIds.cend(), ids.cbegin()));

            AND_WHEN("A timestamp is updated")
            {
                constexpr auto idToUpdate{3};
                const TodoProperties propertiesToUpdate{{"timestamp", 1800}};
                store.update(idToUpdate, propertiesToUpdate);

                THEN("The query do not include the updated todo id")
                {
                    auto ids = store.rangeQuery(minTimeStamp, maxTimeStamp);
                    expectedIds = {2};
                    REQUIRE(ids == expectedIds);
                }

                THEN("Range query including the new timestamp includes the updated id")
                {
                    minTimeStamp = 1500;
                    maxTimeStamp = 2000;
                    ids = store.rangeQuery(minTimeStamp, maxTimeStamp);
                    expectedIds = {3};
                    REQUIRE(ids == expectedIds);
                }
            }

            AND_WHEN("An id is removed")
            {
                store.remove(2);

                THEN("The range query does not include that id")
                {
                    expectedIds = {3};
                    ids = store.rangeQuery(minTimeStamp, maxTimeStamp);
                    REQUIRE(ids == expectedIds);
                }
            }
        }
    }
}