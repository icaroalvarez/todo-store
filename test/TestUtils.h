#pragma once

#include <ParentStore.h>
#include "Todo.h"

using namespace std::string_literals;

namespace TestUtils
{
    bool compareTodoProperties(const TodoProperties& lhs, const TodoProperties& rhs);

    TodoProperties createProperties(std::string title, std::string description, double timestamp);

    ParentStore createDummyParentStore();
}