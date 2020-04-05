
#include "DoublePropertyIds.h"

void DoublePropertyIds::insert(double property, std::int64_t id)
{
    // logarithmic complexity Nlog(size+N) (implementations may optimize if the range is already sorted)
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
    remove(oldPropertyValue, id); // Best case O(1), worst case O(log n)
    insert(newPropertyValue, id); // Complexity O(log n)
}

void DoublePropertyIds::remove(double property, std::int64_t id)
{
    // find has logarithmic complexity O(log n)
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


