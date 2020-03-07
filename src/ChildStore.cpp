#include <vector>
#include "ChildStore.h"

ChildStore::ChildStore(const std::shared_ptr<Store>& parent)
        :parent{parent}
{
}

void ChildStore::insert(std::int64_t id, const TodoProperties& properties)
{
    todosToBeInserted[id]=properties;

    const auto title{std::get<std::string>(properties.at("title"))};
    titleIds[title].push_back(id);

    const auto timestamp{std::get<double>(properties.at("timestamp"))};
    timestampIds[timestamp].push_back(id);
}

void ChildStore::update(std::int64_t id, const TodoProperties &properties)
{
    if(propertiesToBeUpdated.find(id) == propertiesToBeUpdated.end())
    {
        propertiesToBeUpdated[id]=properties;
    } else
    {
        for(auto& property : properties)
        {
            propertiesToBeUpdated[id][property.first] = property.second;
        }
    }
}

TodoProperties ChildStore::get(std::int64_t id) const
{
    TodoProperties properties;
    if(todosToBeInserted.find(id) not_eq todosToBeInserted.end())
    {
        properties = todosToBeInserted.at(id);
    }else{
        if(auto parentSharedPtr{parent.lock()})
        {
            properties = parentSharedPtr->get(id);

            if(propertiesToBeUpdated.find(id) not_eq propertiesToBeUpdated.end())
            {
                for(auto& property : propertiesToBeUpdated.at(id))
                {
                    properties[property.first] = property.second;
                }
            }
        }
    }
    return properties;
}

void ChildStore::remove(std::int64_t id)
{
    idsToBeRemoved.insert(id);
}

bool ChildStore::checkId(std::int64_t id)
{
    auto existInParent{false};
    if(auto parentSharedPtr{parent.lock()})
    {
        existInParent = parentSharedPtr->checkId(id);
    }
    const auto existInToBeInserted{todosToBeInserted.find(id) not_eq todosToBeInserted.end()};
    const auto existInToBeRemoved{idsToBeRemoved.find(id) not_eq idsToBeRemoved.end()};
    return (existInParent or existInToBeInserted) and not existInToBeRemoved;
}

std::vector<std::int64_t> ChildStore::query(const TodoProperty& property) const
{
    std::vector<std::int64_t> ids;
    if(auto parentSharedPtr{parent.lock()})
    {
        ids = parentSharedPtr->query(property);
    }

    if(property.first == "title")
    {
        const auto& title{std::get<std::string>(property.second)};
        if(titleIds.find(title) not_eq titleIds.end())
        {
            auto temp{titleIds.at(title)};
            ids.insert(ids.cend(), temp.cbegin(), temp.cend());
        }
    }

    return ids;
}

std::vector<std::int64_t> ChildStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    std::vector<std::int64_t> ids;
    if(auto parentSharedPtr{parent.lock()})
    {
        ids = parentSharedPtr->rangeQuery(minTimeStamp, maxTimeStamp);
    }

    const auto startIterator{timestampIds.lower_bound(minTimeStamp)};
    const auto endIterator{timestampIds.upper_bound(maxTimeStamp)};

    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        ids.insert(ids.cend(), it->second.cbegin(), it->second.cend());
    }

    return ids;
}

std::shared_ptr<Store> ChildStore::createChild()
{
    throw std::runtime_error("Child store cannot create more children");
}

void ChildStore::commit()
{
    auto parentSharedPtr{parent.lock()};
    if(parentSharedPtr)
    {
        for(const auto& todo : todosToBeInserted)
        {
            parentSharedPtr->insert(todo.first, todo.second);
        }

        for(const auto& todo : propertiesToBeUpdated)
        {
            parentSharedPtr->update(todo.first, todo.second);
        }

        for(const auto& id : idsToBeRemoved)
        {
            parentSharedPtr->remove(id);
        }
    }
}
