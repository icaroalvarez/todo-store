#include <catch2/catch.hpp>
#include <Store.h>
#include <Todo.h>
using namespace std::string_literals;

bool compareTodoProperties(const TodoProperties& lhs, const TodoProperties& rhs)
{
    const auto sameTitle{std::any_cast<std::string>(lhs.at("title")) ==
            std::any_cast<std::string>(rhs.at("title"))};
    const auto sameDescription{std::any_cast<std::string>(lhs.at("description")) ==
                         std::any_cast<std::string>(rhs.at("description"))};
    const auto sameTimestamp{std::any_cast<double>(lhs.at("timestamp")) ==
                         std::any_cast<double>(rhs.at("timestamp"))};
    return sameTitle && sameDescription && sameTimestamp;
}

TEST_CASE("Basic store")
{
    constexpr auto id{2133};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};

    Store store;
    store.insert(id, properties);

    auto retrievedProperties{store.get(id)};
    REQUIRE(compareTodoProperties(retrievedProperties, properties));

    const TodoProperties propertiesToUpdate{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    store.update(id, propertiesToUpdate);
    retrievedProperties = store.get(id);
    REQUIRE(compareTodoProperties(retrievedProperties, propertiesToUpdate));

    store.remove(id);
    const auto idExitsInStore{store.checkId(id)};
    REQUIRE_FALSE(idExitsInStore);
};

Store createDummyStore()
{
    Store store;
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
    store.insert(id++, {{"title", "Call mom"s},
                     {"description", "is her birthday"s},
                     {"timestamp", 1200.0}});
    return store;
}

TEST_CASE("Store queries")
{
    Store store{createDummyStore()};

    const TodoProperty queryProperty{"title", "Buy Milk"s};
    auto setIds{store.query(queryProperty)};
    REQUIRE(std::is_permutation(setIds.cbegin(), setIds.cend(), std::vector<std::int64_t>{0, 1}.cbegin()));

    constexpr auto minTimeStamp{1000.0};
    constexpr auto maxTimeStamp{1300.0};
    setIds = store.rangeQuery(minTimeStamp, maxTimeStamp);
    REQUIRE(std::is_permutation(setIds.cbegin(), setIds.cend(), std::vector<std::int64_t>{2, 3}.cbegin()));
}

TEST_CASE("Child stores")
{
    constexpr auto id{2133};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    Store store;
    store.insert(id, properties);

    Store child{store.createChild()};

    auto retrievedProperties{child.get(id)};
    REQUIRE(compareTodoProperties(retrievedProperties, properties));

    const TodoProperties propertiesAfterUpdate{{"title", "Buy Milk"s},
                                            {"description", "make of almonds!"s},
                                            {"timestamp", 2392348.12233}};
    child.update(id, {{"title", "Buy Milk"s}});
    retrievedProperties = child.get(id);
    REQUIRE(compareTodoProperties(retrievedProperties, propertiesAfterUpdate));

    retrievedProperties = store.get(id);
    REQUIRE(compareTodoProperties(retrievedProperties, properties));

    child.commit();
    retrievedProperties = store.get(id);
    REQUIRE(compareTodoProperties(retrievedProperties, propertiesAfterUpdate));
}
