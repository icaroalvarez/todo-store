#pragma once
#include <unordered_set>
#include <unordered_map>

class StringPropertyIds
{
public:
    void insert(const std::string& property, std::int64_t id);
    std::unordered_set<std::int64_t> getIds(const std::string& property) const;
    void updateProperty(const std::string &oldProperty,
                        const std::string &newProperty,
                        std::int64_t id);
    void remove(const std::string& property, std::int64_t id);
private:
    std::unordered_map<std::string, std::unordered_set<std::int64_t>> propertyIds;
};



