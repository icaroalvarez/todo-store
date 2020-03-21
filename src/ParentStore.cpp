#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto& title{std::get<std::string>(properties.at(titleKey))};
    const auto& description{std::get<std::string>(properties.at(descriptionKey))};
    const auto& timestamp{std::get<double>(properties.at(timestampKey))};
    todos[id]=Todo{id, title, description, timestamp}; // Complexity O(1), O(N) if rehashing is needed.

    /**
     * lets keep a track of the ids related to title and timestamp in order to improve queries performance
     */
    titleIds.insert(title, id); // Complexity O(1), O(N) if rehashing is needed.
    timestampIds.insert(timestamp, id); // Complexity O(log n)
}

void ParentStore::update(std::int64_t id, const TodoProperties &properties)
{
    /**
     * Update also the id from titleIds and timestampIds!
     */
    for(const auto& property : properties)
    {
        if(property.first == titleKey)
        {
            const auto& newTitle{std::get<std::string>(property.second)};
            const auto oldTitle{std::move(todos.at(id).title)};
            todos.at(id).title = newTitle;
            titleIds.updateProperty(oldTitle, newTitle, id);
        }else if(property.first == descriptionKey)
        {
            todos.at(id).description = std::get<std::string>(property.second);
        }else if(property.first == timestampKey)
        {
            const auto& newTimestamp{std::get<double>(property.second)};
            const auto& oldTimestamp{todos.at(id).timestamp};
            timestampIds.updateProperty(oldTimestamp, newTimestamp, id);
            todos.at(id).timestamp = newTimestamp;
        }else{
            throw std::invalid_argument("Unknown property: "+std::string(property.first));
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
    const auto todoExits{todos.find(id) not_eq todos.end()};
    if(todoExits)
    {
        /**
         * Remove also the id from titleIds and timestamp ids.
         */
        const auto &title{todos.at(id).title};
        titleIds.remove(title, id); // Complexity O(1), worst case O(N)

        const auto &timestamp{todos.at(id).timestamp};
        timestampIds.remove(timestamp, id); // Best case O(1), worst case O(log n)

        todos.erase(id); // Complexity average O(1), worst case O(N)
    }
}

bool ParentStore::checkId(std::int64_t id)
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

std::shared_ptr<Store> ParentStore::createChild()
{
    /**
     * Children have a pointer to parent so they can get todos and performance id queries
     * from the parent without the need to copy all the parent todos into the child.
     */
    return std::make_shared<ChildStore>(shared_from_this());
}

void ParentStore::commit()
{
    throw std::runtime_error("Parent store cannot commit, only child stores can");
}
