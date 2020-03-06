#include <vector>
#include "Store.h"

void Store::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto title{std::any_cast<std::string>(properties.at("title"))};
    const auto description{std::any_cast<std::string>(properties.at("description"))};
    const auto timestamp{std::any_cast<double>(properties.at("timestamp"))};
    todos[id]=Todo{id, title, description, timestamp};
}

void Store::update(std::int64_t id, const TodoProperties &properties)
{
    for(const auto& property : properties)
    {
        if(property.first == "title")
        {
            todos[id].title = std::any_cast<std::string>(property.second);
        }else if(property.first == "description")
        {
            todos[id].description = std::any_cast<std::string>(property.second);
        }else if(property.first == "timestamp")
        {
            todos[id].timestamp = std::any_cast<double>(property.second);
        }else{
            throw std::invalid_argument("Unknown property: "+property.first);
        }
    }
}

TodoProperties Store::get(std::int64_t id) const
{
    TodoProperties todoProperties;
    todoProperties["title"]=todos.at(id).title;
    todoProperties["description"]=todos.at(id).description;
    todoProperties["timestamp"]=todos.at(id).timestamp;
    return todoProperties;
}

void Store::remove(std::int64_t id)
{
    todos.erase(id);
}

bool Store::checkId(std::int64_t id)
{
    return todos.find(id) not_eq todos.end();
}

std::vector<std::int64_t> Store::query(const TodoProperty& property) const
{
    std::vector<std::int64_t> ids;
    for(const auto& todo : todos)
    {
        auto matches{false};
        if(property.first == "title")
        {
            matches = todo.second.title == std::any_cast<std::string>(property.second);
        }else if(property.first == "description")
        {
            matches = todo.second.description == std::any_cast<std::string>(property.second);
        }else if(property.first == "timestamp")
        {
            matches = todo.second.timestamp == std::any_cast<double>(property.second);
        }
        if(matches)
        {
            ids.push_back(todo.first);
        }
    }
    return ids;
}

std::vector<std::int64_t> Store::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    std::vector<std::int64_t> ids;
    for(const auto& todo : todos)
    {
        const auto inRange{todo.second.timestamp >= minTimeStamp and todo.second.timestamp <= maxTimeStamp};
        if(inRange)
        {
            ids.push_back(todo.first);
        }
    }
    return ids;
}

Store Store::createChild() const
{
    Store store{*this};
    return store;
}

void Store::commit()
{
    if(parent)
    {
        for(const auto& todo : todos)
        {
            const auto properties{get(todo.first)};
            parent->update(todo.first, properties);
        }
    }
}
