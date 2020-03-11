#pragma once
#include <unordered_set>
#include <unordered_map>
#include <map>

/**
 * Responsibility: keep a set of ids related to a double property so
 * it can be retrieved in a fast way when querying ids giving a specific range
 */
class DoublePropertyIds
{
public:
    void insert(double property, std::int64_t id);

    /**
     * Here we couldn't return a const& because the set has to be created depending of the range.
     * An alternative would be to create a view using C++20 range features (or rangeV3, boost).
     */
    std::unordered_set<std::int64_t> getRangeIds(double minValue, double maxValue) const;

    void updateProperty(double oldPropertyValue,
                        double newPropertyValue,
                        std::int64_t id);

    void remove(double property, std::int64_t id);
private:
    /**
     * A sorted container is more convenient than an unordered one to improve
     * the performance while searching ranges (from min value to mas value).
     * A multi map is more convenient because more than one id can have the
     * same timestamp and makes it faster to create the output set when calling
     * rangeQuery that having a std::multimap<double, std::unordered_set<std::int64_t>.
     */
    std::multimap<double, std::int64_t> propertyIds;
};



