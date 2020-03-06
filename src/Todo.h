#pragma once
#include <string>
#include <any>
#include <unordered_map>

using TodoProperty = std::pair<std::string, std::any>;
using TodoProperties = std::unordered_map<std::string, std::any>;

struct Todo
{
    std::int64_t id;
    std::string title;
    std::string description;
    double timestamp;
    // and many more in the shipping app
};
