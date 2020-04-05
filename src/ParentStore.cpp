#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto titleIt{properties.find(titleKey)};
    const auto titlePresent{titleIt not_eq properties.end()};
    const auto descriptionIt{properties.find(descriptionKey)};
    const auto descriptionPresent{descriptionIt not_eq properties.end()};
    const auto timestampIt{properties.find(timestampKey)};
    const auto timestampPresent{timestampIt not_eq properties.end()};
    const auto allPropertiesPresent{titlePresent and descriptionPresent and timestampPresent};
    if(not allPropertiesPresent)
    {
        throw std::invalid_argument("Missing properties when inserting a todo in the store. "
                                    "Please review that all properties are specified");
    }
    const auto& title{std::get<std::string>(titleIt->second)};
    const auto& description{std::get<std::string>(descriptionIt->second)};
    const auto& timestamp{std::get<double>(timestampIt->second)};
    todos[id]=Todo{id, title, description, timestamp}; // Complexity O(1), O(N) if rehashing is needed.

    /**
     * lets keep a track of the ids related to title and timestamp in order to improve queries performance
     */
    titleIds.insert(title, id); // Complexity O(1), O(N) if rehashing is needed.
    timestampIds.insert(timestamp, id); // Complexity O(log n)
}

void ParentStore::update(std::int64_t id, const TodoProperties& properties)
{
    auto todoIt{todos.find(id)};
    const auto idExists{todoIt not_eq todos.end()};

    if(not idExists)
    {
        throw std::invalid_argument("Error updating properties. "
                                    "Todo with id "+std::to_string(id)+" not found");
    }

    for (const auto& property : properties)
    {
        if (property.first == titleKey)
        {
            const auto& newTitle{std::get<std::string>(property.second)};
            const auto oldTitle{std::move(todoIt->second.title)};
            todoIt->second.title = newTitle;
            titleIds.updateProperty(oldTitle, newTitle, id);
        } else if (property.first == descriptionKey)
        {
            todoIt->second.description = std::get<std::string>(property.second);
        } else if (property.first == timestampKey)
        {
            const auto& newTimestamp{std::get<double>(property.second)};
            const auto& oldTimestamp{todoIt->second.timestamp};
            timestampIds.updateProperty(oldTimestamp, newTimestamp, id);
            todoIt->second.timestamp = newTimestamp;
        } else
        {
            throw std::invalid_argument("Unknown property: " + std::string(property.first));
        }
    }
}

TodoProperties ParentStore::get(std::int64_t id) const
{
    const auto& todo{todos.at(id)};
    return {
            {titleKey,       todo.title},
            {descriptionKey, todo.description},
            {timestampKey,   todo.timestamp}
    };
}

void ParentStore::remove(std::int64_t id)
{
    // find todos complexity O(1), worst case O(N)
    auto it{todos.find(id)};
    const auto todoExits{it not_eq todos.end()};
    if(not todoExits)
    {
        throw std::invalid_argument("Error removing todo. "
                                    "Todo with id "+std::to_string(id)+" not found");
    }
    const auto &title{it->second.title};
    titleIds.remove(title, id); // Complexity constant O(1), worst case O(N)

    const auto &timestamp{it->second.timestamp};
    timestampIds.remove(timestamp, id); // Complexity logarithmic O(log n)

    todos.erase(it); // Complexity constant O(1)
}

bool ParentStore::checkId(std::int64_t id) const
{
    return todos.find(id) not_eq todos.end();
}

std::unordered_set<std::int64_t> ParentStore::query(const TodoProperty& property) const
{
    /**
     * Only title is supported but supporting also description is very trivial.
     */
    std::unordered_set<std::int64_t> ids;
    if(property.first == titleKey)
    {
        const auto& title{std::get<std::string>(property.second)};
        ids = titleIds.getIds(title);
    }
    return ids;
}

std::unordered_set<std::int64_t> ParentStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    return timestampIds.getRangeIds(minTimeStamp, maxTimeStamp);
}

std::unique_ptr<Store> ParentStore::createChild()
{
    /**
     * Children have a pointer to parent so they can get todos and performance id queries
     * from the parent without the need to copy all the parent todos into the child.
     */
    return std::make_unique<ChildStore>(shared_from_this());
}

void ParentStore::commit()
{
    throw std::runtime_error("Parent store cannot commit, only child stores can");
}
