
#include "DoublePropertyIds.h"

void DoublePropertyIds::insert(double property, std::int64_t id)
{
    propertyIds.insert({property, id});
}

std::unordered_set<std::int64_t> DoublePropertyIds::getRangeIds(double minValue, double maxValue) const
{
    const auto& startIterator{propertyIds.lower_bound(minValue)};
    const auto& endIterator{propertyIds.upper_bound(maxValue)};

    std::unordered_set<std::int64_t> ids;
    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        ids.emplace(it->second);
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
    const auto& iterPair{propertyIds.equal_range(property)};
    for (auto it = iterPair.first; it != iterPair.second; ++it)
    {
        if (it->second == id)
        {
            propertyIds.erase(it);
            break;
        }
    }
}


