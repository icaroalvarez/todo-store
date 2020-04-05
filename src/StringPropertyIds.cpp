#include "StringPropertyIds.h"

void StringPropertyIds::insert(const std::string& property, std::int64_t id)
{
    // Complexity O(1), O(N) if rehashing is needed.
    propertyIds[property].insert(id);
}

std::unordered_set<std::int64_t> StringPropertyIds::getIds(const std::string& property) const
{
    // Find complexity average constant O(1), worst case O(N) (In case all keys are in same bucket)
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
    // Find complexity average constant O(1), worst case O(N) (In case all keys are in same bucket)
    auto it{propertyIds.find(property)};
    const auto propertyExists{it not_eq propertyIds.end()};
    if(propertyExists)
    {
        if (it->second.size() > 1)
        {
            it->second.erase(id); // Complexity O(1), worst case O(N)
        } else
        {
            propertyIds.erase(it); // Complexity O(1)
        }
    }
}
