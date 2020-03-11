#pragma once
#include <unordered_set>
#include <unordered_map>

/**
 * Responsibility: keep a set of ids related to a string property so
 * it can be retrieved in a fast way when querying ids giving a specific string property
 */
class StringPropertyIds
{
public:
    void insert(const std::string& property, std::int64_t id);

    /**
     * We could return here a const& in order to avoid an extra copying
     * In this case the client could only read the ids.
     * Another alternative would be create a view with C++20 features (or rangeV3, boost)
     */
    std::unordered_set<std::int64_t> getIds(const std::string& property) const;

    void updateProperty(const std::string &oldProperty,
                        const std::string &newProperty,
                        std::int64_t id);

    void remove(const std::string& property, std::int64_t id);
private:
     /**
      * Unordered container was chosen because it performance better and there is not need
      * to have the ids in a specific order
      */
    std::unordered_map<std::string, std::unordered_set<std::int64_t>> propertyIds;
};



