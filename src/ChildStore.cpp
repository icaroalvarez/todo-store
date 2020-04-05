#include <vector>
#include "ChildStore.h"

ChildStore::ChildStore(std::shared_ptr<Store> parent)
        :parent{std::move(parent)}
{
}

void ChildStore::insert(std::int64_t id, const TodoProperties& properties)
{
    todosToBeInserted[id]=properties;

    // insert the id into the titleIds
    const auto& title{std::get<std::string>(properties.at(titleKey))};
    titleIds.insert(title, id);

    // insert the id into the timestampIds
    const auto timestamp{std::get<double>(properties.at(timestampKey))};
    timestampIds.insert(timestamp, id);
}

void ChildStore::update(std::int64_t id, const TodoProperties& properties)
{
    const auto alreadyExists{propertiesToBeUpdated.find(id) not_eq propertiesToBeUpdated.end()};
    if(alreadyExists)
    {
        for(const auto& property : properties)
        {
            propertiesToBeUpdated.at(id)[property.first] = property.second;
        }
    } else
    {
        propertiesToBeUpdated[id]=properties;
    }

    const auto hasTitleProperty{properties.find(titleKey) not_eq properties.end()};
    if(hasTitleProperty)
    {
        const auto oldTitle{std::get<std::string>(parent->get(id).at(titleKey))};
        oldTitleIdsToBeUpdated[oldTitle].insert(id);
        const auto& newTitle{std::get<std::string>(properties.at(titleKey))};
        titleIds.insert(newTitle, id);
    }

    const auto hasTimestampProperty{properties.find(timestampKey) not_eq properties.end()};
    if(hasTimestampProperty)
    {
        const auto oldTimestamp{std::get<double>(parent->get(id).at(timestampKey))};
        oldTimestampIdsToBeUpdated.insert({oldTimestamp, id});
        const auto timestamp{std::get<double>(properties.at(timestampKey))};
        timestampIds.insert(timestamp, id);
    }
}

TodoProperties ChildStore::get(std::int64_t id) const
{
    TodoProperties properties;
    const auto idWillBeInserted{todosToBeInserted.find(id) not_eq todosToBeInserted.end()};
    if(idWillBeInserted)
    {
        // if the id is going to be inserted in the child, just return the properties
        properties = todosToBeInserted.at(id);
    }else{
        const auto idWillBeRemoved{todosToBeRemoved.find(id) not_eq todosToBeRemoved.end()};
        if (not idWillBeRemoved)
        {
            // return to-do properties from the parent if the id is not included in the ones to remove in the child
            properties = parent->get(id);

            // if the id is about to be updated return the new properties instead of the ones from the parent
            const auto idWillBeUpdated{propertiesToBeUpdated.find(id) not_eq propertiesToBeUpdated.end()};
            if (idWillBeUpdated)
            {
                for (auto& property : propertiesToBeUpdated.at(id))
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
    // keep track of the id to be removed into the parent when commit the child
    todosToBeRemoved.insert(id);
}

bool ChildStore::checkId(std::int64_t id) const
{
    const auto existInParent{parent->checkId(id)};
    const auto existInToBeInserted{todosToBeInserted.find(id) not_eq todosToBeInserted.end()};
    const auto existInToBeRemoved{todosToBeRemoved.find(id) not_eq todosToBeRemoved.end()};
    return (existInParent or existInToBeInserted) and not existInToBeRemoved;
}

std::unordered_set<std::int64_t> ChildStore::query(const TodoProperty& property) const
{
    auto ids{parent->query(property)};

    // remove the ids that are going to be removed from the parent set
    for(const auto& idToBeRemoved : todosToBeRemoved)
    {
        ids.erase(idToBeRemoved);
    }

    if(property.first == titleKey)
    {
        // add the titles that are going to be inserted or updated in the child
        const auto& title{std::get<std::string>(property.second)};
        auto childIds{titleIds.getIds(title)};
        if(not childIds.empty())
        {
            ids.merge(std::move(childIds));
        }

        // remove the ids from old title that are going to be updated in the child
        const auto titleWillBeUpdated{oldTitleIdsToBeUpdated.find(title) not_eq oldTitleIdsToBeUpdated.end()};
        if(titleWillBeUpdated)
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
    auto ids{parent->rangeQuery(minTimeStamp, maxTimeStamp)};

    // remove the ids from the parent that is going to be removed in the child
    for(const auto& idToBeRemoved : todosToBeRemoved)
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
    ids.insert(childIds.cbegin(), childIds.cend());
    return ids;
}

std::unique_ptr<Store> ChildStore::createChild()
{
    throw std::runtime_error("Child store cannot create children");
}

void ChildStore::commit()
{
    /**
     * Perform all the saved actions on the parent when committing
     */
    for(const auto& todo : todosToBeInserted)
    {
        parent->insert(todo.first, todo.second);
    }

    for(const auto& todo : propertiesToBeUpdated)
    {
        parent->update(todo.first, todo.second);
    }

    for(const auto& id : todosToBeRemoved)
    {
        parent->remove(id);
    }
}
