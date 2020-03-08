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
    timestampIds[timestamp].insert(id);
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
            const auto oldTimestamp{todos[id].timestamp};
            todos[id].timestamp = timestamp;
            timestampIds.at(oldTimestamp).erase(id);
            timestampIds[timestamp].insert(id);
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
    const auto& todoTitle{todos[id].title};
    auto& tempTitleIds{titleIds[todoTitle]};
    if(tempTitleIds.size() > 1)
    {
        tempTitleIds.erase(id);
    }else{
        titleIds.erase(todoTitle);
    }

    const auto& todoTimestamp{todos[id].timestamp};
    auto& tempTimestampIds{timestampIds[todoTimestamp]};
    if(tempTimestampIds.size() > 1)
    {
        tempTimestampIds.erase(id);
    }else{
        timestampIds.erase(todoTimestamp);
    }

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
        auto tempIds{it->second};
        ids.merge(std::move(tempIds));
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
