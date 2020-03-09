#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto title{std::get<std::string>(properties.at("title"))};
    const auto description{std::get<std::string>(properties.at("description"))};
    const auto timestamp{std::get<double>(properties.at("timestamp"))};
    todos[id]=Todo{id, title, description, timestamp};
    titleIds[title].insert(id);
    timestampIds.insert({timestamp, id});
}

void removeTimestampId(double timestamp, std::int64_t id, std::multimap<double, std::int64_t>& timestampIds)
{
    const auto& iterPair{timestampIds.equal_range(timestamp)};
    for (auto it = iterPair.first; it != iterPair.second; ++it)
    {
        if (it->second == id)
        {
            timestampIds.erase(it);
            break;
        }
    }
}

void ParentStore::update(std::int64_t id, const TodoProperties &properties)
{
    for(const auto& property : properties)
    {
        if(property.first == "title")
        {
            const auto& newTitle{std::get<std::string>(property.second)};
            const auto oldTitle{std::move(todos[id].title)};
            todos[id].title = newTitle;
            titleIds.at(oldTitle).erase(id);
            titleIds[newTitle].insert(id);
        }else if(property.first == "description")
        {
            todos[id].description = std::get<std::string>(property.second);
        }else if(property.first == "timestamp")
        {
            const auto& timestamp{std::get<double>(property.second)};
            removeTimestampId(todos[id].timestamp, id, timestampIds);
            todos[id].timestamp = timestamp;
            timestampIds.insert({timestamp, id});
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
    const auto& todoTitle{todos[id].title};
    auto& tempTitleIds{titleIds[todoTitle]};
    if(tempTitleIds.size() > 1)
    {
        tempTitleIds.erase(id);
    }else{
        titleIds.erase(todoTitle);
    }
    removeTimestampId(todos[id].timestamp, id, timestampIds);
    todos.erase(id);
}

bool ParentStore::checkId(std::int64_t id)
{
    return todos.find(id) not_eq todos.end();
}

std::unordered_set<std::int64_t> ParentStore::query(const TodoProperty& property) const
{
    std::unordered_set<std::int64_t> ids;
    if(property.first == "title")
    {
        ids = titleIds.at(std::get<std::string>(property.second));
    }
    return ids;
}

std::unordered_set<std::int64_t> ParentStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    std::unordered_set<std::int64_t> ids;

    const auto startIterator{timestampIds.lower_bound(minTimeStamp)};
    const auto endIterator{timestampIds.upper_bound(maxTimeStamp)};

    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        ids.emplace(it->second);
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
