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
    titleIds.insert(title, id);

    const auto timestamp{std::get<double>(properties.at("timestamp"))};
    timestampIds.insert(timestamp, id);
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

    if(properties.find("title") not_eq properties.end())
    {
        if(auto parentSharedPtr{parent.lock()})
        {
            const auto oldTitle{std::get<std::string>(parentSharedPtr->get(id).at("title"))};
            const auto newTitle{std::get<std::string>(properties.at("title"))};
            oldTitleIdsToBeUpdated[oldTitle].insert(id);
            titleIds.insert(newTitle, id);
        }
    }

    if(properties.find("timestamp") not_eq properties.end())
    {
        if(auto parentSharedPtr{parent.lock()})
        {
            const auto oldTimestamp{std::get<double>(parentSharedPtr->get(id).at("timestamp"))};
            oldTimestampIdsToBeUpdated.insert({oldTimestamp, id});
            const auto timestamp{std::get<double>(properties.at("timestamp"))};
            timestampIds.insert(timestamp, id);
        }
    }
}

TodoProperties ChildStore::get(std::int64_t id) const
{
    TodoProperties properties;
    const auto idWillBeInserted{todosToBeInserted.find(id) not_eq todosToBeInserted.end()};
    if(idWillBeInserted)
    {
        properties = todosToBeInserted.at(id);
    }else{
        const auto idWillBeRemoved{idsToBeRemoved.find(id) not_eq idsToBeRemoved.end()};
        auto parentSharedPtr{parent.lock()};
        if (parentSharedPtr && not idWillBeRemoved)
        {
            properties = parentSharedPtr->get(id);
            const auto idWillBeUpdated{propertiesToBeUpdated.find(id) not_eq propertiesToBeUpdated.end()};
            if (idWillBeUpdated)
            {
                for (auto &property : propertiesToBeUpdated.at(id))
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

std::unordered_set<std::int64_t> ChildStore::query(const TodoProperty& property) const
{
    /**
     * Here we cannot return a const reference of the parent store set (titleIds) because
     * we need to modified with the ids inserted, modified or removed in the child
     * What we could do is to copy the parent set but in a low priority thread just after
     * creating the child, so it could do the copy in the background in order to have a
     * copy already created in the moment of executing a query
     */

    std::unordered_set<std::int64_t> ids;
    if(auto parentSharedPtr{parent.lock()})
    {
        ids = std::move(parentSharedPtr->query(property));
    }

    for(const auto& idToBeRemoved : idsToBeRemoved)
    {
        ids.erase(idToBeRemoved);
    }

    if(property.first == "title")
    {
        const auto& title{std::get<std::string>(property.second)};
        auto childIds{titleIds.getIds(title)};
        if(not childIds.empty())
        {
            ids.merge(std::move(childIds));
        }

        if(oldTitleIdsToBeUpdated.find(title) not_eq oldTitleIdsToBeUpdated.end())
        {
            for(const auto& id : oldTitleIdsToBeUpdated.at(title))
            {
                ids.erase(id);
            }
        }
    }
    return ids;
}

std::unordered_set<std::int64_t> ChildStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    std::unordered_set<std::int64_t> ids;
    if(auto parentSharedPtr{parent.lock()})
    {
        ids = std::move(parentSharedPtr->rangeQuery(minTimeStamp, maxTimeStamp));
    }

    // remove the ids from the parent that is going to be removed in the child
    for(const auto& idToBeRemoved : idsToBeRemoved)
    {
        ids.erase(idToBeRemoved);
    }

    // remove the ids from the parent that are going to be updated in the child
    auto startIterator{oldTimestampIdsToBeUpdated.lower_bound(minTimeStamp)};
    auto endIterator{oldTimestampIdsToBeUpdated.upper_bound(maxTimeStamp)};
    for(auto it=startIterator; it != endIterator; std::advance(it, 1))
    {
        ids.erase(it->second);
    }

    // add the new ids inserted and the ones updated in the child
    const auto& childIds{timestampIds.getRangeIds(minTimeStamp, maxTimeStamp)};
    ids.insert(childIds.begin(), childIds.end());

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
