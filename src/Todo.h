#pragma once
#include <string>
#include <variant>
#include <unordered_map>

using TodoProperty = std::pair<std::string, std::variant<std::string, double>>;
using TodoProperties = std::unordered_map<std::string, std::variant<std::string, double>>;

struct Todo
{
    std::int64_t id;
    std::string title;
    std::string description;
    double timestamp;
    // and many more in the shipping app
};
