
#include "DoublePropertyIds.h"

void DoublePropertyIds::insert(double property, std::int64_t id)
{
    propertyIds[property].insert(id);
}

std::unordered_set<std::int64_t> DoublePropertyIds::getRangeIds(double minValue, double maxValue) const
{
    const auto& startIterator{propertyIds.lower_bound(minValue)};
    const auto& endIterator{propertyIds.upper_bound(maxValue)};

    std::unordered_set<std::int64_t> ids;
    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        auto timeStampIds{it->second};
        ids.merge(timeStampIds);
    }
    return ids;
}

void DoublePropertyIds::updateProperty(double oldPropertyValue,
                                       double newPropertyValue,
                                       std::int64_t id)
{
    remove(oldPropertyValue, id);
    insert(newPropertyValue, id);
}

void DoublePropertyIds::remove(double property, std::int64_t id)
{
    const auto propertyExists{propertyIds.find(property) not_eq propertyIds.end()};
    if(propertyExists)
    {
        auto& ids{propertyIds.at(property)};
        if (ids.size() > 1)
        {
            ids.erase(id);
        } else
        {
            propertyIds.erase(property);
        }
    }
}


