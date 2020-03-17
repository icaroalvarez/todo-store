#pragma once
#include <string>
#include <variant>
#include <unordered_map>

constexpr std::string_view titleKey{"title"};
constexpr std::string_view descriptionKey{"title"};
constexpr std::string_view timestampKey{"timestamp"};

using TodoProperty = std::pair<std::string_view, std::variant<std::string, double>>;
using TodoProperties = std::unordered_map<std::string_view, std::variant<std::string, double>>;

struct Todo
{
    std::int64_t id;
    std::string title;
    std::string description;
    double timestamp;
    // and many more in the shipping app
};
