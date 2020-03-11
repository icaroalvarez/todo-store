#include "StringPropertyIds.h"

void StringPropertyIds::insert(const std::string& property, std::int64_t id)
{
    propertyIds[property].insert(id);
}

std::unordered_set<std::int64_t> StringPropertyIds::getIds(const std::string& property) const
{
    std::unordered_set<std::int64_t> ids;
    if(propertyIds.find(property) not_eq propertyIds.end())
    {
        ids = std::move(propertyIds.at(property));
    }
    return ids;
}

void StringPropertyIds::updateProperty(const std::string &oldProperty,
                                       const std::string &newProperty,
                                       std::int64_t id)
{
    remove(oldProperty, id);
    insert(newProperty, id);
}

void StringPropertyIds::remove(const std::string &property, std::int64_t id)
{
    auto& ids{propertyIds.at(property)};
    if(ids.size() > 1)
    {
        ids.erase(id);
    }else{
        propertyIds.erase(property);
    }
}
