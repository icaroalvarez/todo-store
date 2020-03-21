#include "StringPropertyIds.h"

void StringPropertyIds::insert(const std::string& property, std::int64_t id)
{
    // Complexity O(1), O(N) if rehashing is needed.
    propertyIds[property].insert(id);
}

std::unordered_set<std::int64_t> StringPropertyIds::getIds(const std::string& property) const
{
    std::unordered_set<std::int64_t> ids;
    if(propertyIds.find(property) not_eq propertyIds.end())
    {
        ids = propertyIds.at(property);
    }
    return ids;
}

void StringPropertyIds::updateProperty(const std::string &oldProperty,
                                       const std::string &newProperty,
                                       std::int64_t id)
{
    remove(oldProperty, id); // Complexity O(1), worst case O(N)
    insert(newProperty, id); // Complexity O(1), O(N) if rehashing is needed.
}

void StringPropertyIds::remove(const std::string &property, std::int64_t id)
{
    const auto propertyExists{propertyIds.find(property) not_eq propertyIds.end()};
    if(propertyExists)
    {
        auto &ids{propertyIds.at(property)};
        if (ids.size() > 1)
        {
            ids.erase(id); // Complexity O(1), worst case O(N)
        } else
        {
            propertyIds.erase(property); // Complexity O(1), worst case O(N)
        }
    }
}
