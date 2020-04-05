#include "TestUtils.h"

namespace TestUtils
{
    bool compareTodoProperties(const TodoProperties& lhs, const TodoProperties& rhs)
    {
        const auto sameTitle{std::get<std::string>(lhs.at(titleKey)) ==
                             std::get<std::string>(rhs.at(titleKey))};
        const auto sameDescription{std::get<std::string>(lhs.at(descriptionKey)) ==
                                   std::get<std::string>(rhs.at(descriptionKey))};
        const auto sameTimestamp{std::get<double>(lhs.at(timestampKey)) ==
                                 std::get<double>(rhs.at(timestampKey))};
        return sameTitle && sameDescription && sameTimestamp;
    }

    TodoProperties createProperties(std::string title, std::string description, double timestamp)
    {
        return {
                {titleKey, std::move(title)},
                {descriptionKey, std::move(description)},
                {timestampKey, timestamp}
        };
    }

    ParentStore createDummyParentStore()
    {
        ParentStore store;
        auto id{0};
        auto todoProperty{TestUtils::createProperties("Buy Milk"s,
                                                      "make of almonds!"s, 2392348.12233)};
        store.insert(id++, todoProperty);

        todoProperty = {TestUtils::createProperties("Buy Milk"s,
                                                    "don't forget!"s, 2400050.12555)};
        store.insert(id++, todoProperty);

        todoProperty = {TestUtils::createProperties("Study Chinese"s,
                                                    "worth it!"s, 1000.0)};
        store.insert(id++, todoProperty);

        todoProperty = {TestUtils::createProperties("Call mom"s,
                                                    "is her birthday"s, 1200.0)};
        store.insert(id, todoProperty);
        return store;
    }
}
