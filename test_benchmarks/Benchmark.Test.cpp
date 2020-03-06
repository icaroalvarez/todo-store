#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <Store.h>

using namespace std::string_literals;

TEST_CASE("insertion performance")
{
    constexpr auto id{2133};
    const TodoProperties properties{{"title", "Buy Milk"s},
                                    {"description", "make of almonds!"s},
                                    {"timestamp", 2392348.12233}};
    Store store;

    BENCHMARK("inserting")
                {
                    return store.insert(id, properties);
                };

    const TodoProperties propertiesToUpdate{{"title",       "Buy Milk"s},
                                            {"description", "make of almonds!"s},
                                            {"timestamp",   2392348.12233}};
    BENCHMARK("updating")
                {
                    return store.update(id, propertiesToUpdate);
                };

    BENCHMARK("retrieve properties")
                {
                    return store.get(id);
                };
}
