#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto& title{std::get<std::string>(properties.at("title"))};
    const auto& description{std::get<std::string>(properties.at("description"))};
    const auto& timestamp{std::get<double>(properties.at("timestamp"))};
    todos[id]=Todo{id, title, description, timestamp};

    /**
     * lets keep a track of the ids related to title and timestamp in order to improve queries performance
     */
    titleIds.insert(title, id);
    timestampIds.insert(timestamp, id);
}

void ParentStore::update(std::int64_t id, const TodoProperties &properties)
{
    /**
     * Update also the id from titleIds and timestampIds!
     */
    for(const auto& property : properties)
    {
        if(property.first == "title")
        {
            const auto& newTitle{std::get<std::string>(property.second)};
            const auto oldTitle{std::move(todos.at(id).title)};
            todos.at(id).title = newTitle;
            titleIds.updateProperty(oldTitle, newTitle, id);
        }else if(property.first == "description")
        {
            todos.at(id).description = std::get<std::string>(property.second);
        }else if(property.first == "timestamp")
        {
            const auto& newTimestamp{std::get<double>(property.second)};
            const auto& oldTimestamp{todos.at(id).timestamp};
            timestampIds.updateProperty(oldTimestamp, newTimestamp, id);
            todos.at(id).timestamp = newTimestamp;
        }else{
            throw std::invalid_argument("Unknown property: "+property.first);
        }
    }
}

TodoProperties ParentStore::get(std::int64_t id) const
{
    const auto& todo{todos.at(id)};
    return {
            {"title",       todo.title},
            {"description", todo.description},
            {"timestamp",   todo.timestamp}
    };
}

void ParentStore::remove(std::int64_t id)
{
    /**
     * Remove also the id from titleIds and timestamp ids.
     */
    const auto& title{todos.at(id).title};
    titleIds.remove(title, id);

    const auto& timestamp{todos.at(id).timestamp};
    timestampIds.remove(timestamp, id);

    todos.erase(id);
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
    if(property.first == "title")
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
    auto store{std::make_shared<ChildStore>(shared_from_this())};
    children.push_back(store);
    return store;
}

void ParentStore::commit()
{
    throw std::runtime_error("Parent store cannot commit, only child stores can");
}
