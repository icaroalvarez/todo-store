#include <vector>
#include "ChildStore.h"

ChildStore::ChildStore(const std::shared_ptr<Store>& parent)
        :parent{parent}
{
}

void ChildStore::insert(std::int64_t id, const TodoProperties& properties)
{
    todosToBeInserted[id]=properties;
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
    return existInParent or existInToBeInserted;
}

std::vector<std::int64_t> ChildStore::query(const TodoProperty& property) const
{

}

std::vector<std::int64_t> ChildStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{

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
