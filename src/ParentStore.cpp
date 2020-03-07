#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto title{std::get<std::string>(properties.at("title"))};
    const auto description{std::get<std::string>(properties.at("description"))};
    const auto timestamp{std::get<double>(properties.at("timestamp"))};
    todos[id]=Todo{id, title, description, timestamp};
    titleIds[title].push_back(id);
    timestampIds[timestamp].push_back(id);
}

void ParentStore::update(std::int64_t id, const TodoProperties &properties)
{
    for(const auto& property : properties)
    {
        if(property.first == "title")
        {
            todos[id].title = std::get<std::string>(property.second);
        }else if(property.first == "description")
        {
            todos[id].description = std::get<std::string>(property.second);
        }else if(property.first == "timestamp")
        {
            todos[id].timestamp = std::get<double>(property.second);
        }else{
            throw std::invalid_argument("Unknown property: "+property.first);
        }
    }
}

TodoProperties ParentStore::get(std::int64_t id) const
{
    auto todo{todos.at(id)};
    return {
            {"title",       todo.title},
            {"description", todo.description},
            {"timestamp",   todo.timestamp}
    };
}

void ParentStore::remove(std::int64_t id)
{
    todos.erase(id);
}

bool ParentStore::checkId(std::int64_t id)
{
    return todos.find(id) not_eq todos.end();
}

std::vector<std::int64_t> ParentStore::query(const TodoProperty& property) const
{
    std::vector<std::int64_t> ids;
    if(property.first == "title")
    {
        ids = titleIds.at(std::get<std::string>(property.second));
    }
    return ids;
}

std::vector<std::int64_t> ParentStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    std::vector<std::int64_t> ids;

    const auto startIterator{timestampIds.lower_bound(minTimeStamp)};
    if(startIterator == timestampIds.begin())
    {
        return ids;
    }
    const auto endIterator{timestampIds.upper_bound(maxTimeStamp)};
    if(endIterator == timestampIds.end())
    {
        return ids;
    }

    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        ids.insert(ids.cend(), it->second.cbegin(), it->second.cend());
    }
    return ids;
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
