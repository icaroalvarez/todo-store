#include <vector>
#include "ParentStore.h"
#include "ChildStore.h"

void ParentStore::insert(std::int64_t id, const TodoProperties& properties)
{
    const auto& title{std::get<std::string>(properties.at("title"))};
    const auto& description{std::get<std::string>(properties.at("description"))};
    const auto& timestamp{std::get<double>(properties.at("timestamp"))};
    todos[id]=Todo{id, title, description, timestamp};

    // let's keep the id in a associative container with the title as key in order to improve
    // the performance of future queries
    titleIds[title].insert(id);

    // let's keep the id in a associative container with the timestamp as key in order to improve
    // the performance of future queries
    timestampIds.insert({timestamp, id});
}

/**
 * This is just a helper function ir order to remove the id from the timestampIds associative container.
 * @param timestamp key of the id we want to remove
 * @param id the id to be removed
 * @param timestampIds the associative container
 */
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
            /**
             * Here we want also to update the titleIds associative container
             * removing the id associated with the old title and inserting
             * the id associated with the new title
             */
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
            /**
             * Here we want also to update the timestampIds associative container
             * removing the id associated with the old timestamp and inserting
             * the id associated with the new timestamp
             */
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
    /**
     * Remove also the id from the associative containers titleIds and timestamp ids.
     */
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
    /**
     * Create a set of ids. Here we could improve the efficiency returning a const reference object
     * since we have the container already created (titleIds), avoiding the extra time for creating a copy
     */
    std::unordered_set<std::int64_t> ids;
    if(property.first == "title")
    {
        const auto& title{std::get<std::string>(property.second)};
        if(titleIds.find(title) not_eq titleIds.end())
        {
            ids = titleIds.at(title);
        }
    }
    return ids;
}

std::unordered_set<std::int64_t> ParentStore::rangeQuery(double minTimeStamp, double maxTimeStamp) const
{
    /**
     * Here we need to create a new set since and we cannot return a const reference since we have
     * not a container already created (timestampIds contains all timestamps, not just a range).
     * What we could do is use the range view feature of C++20 (or rangesV3 libraries or boost::ranges) in
     * order to create a view from the container filtering just the values in the range
     */
    const auto& startIterator{timestampIds.lower_bound(minTimeStamp)};
    const auto& endIterator{timestampIds.upper_bound(maxTimeStamp)};

    std::unordered_set<std::int64_t> ids;
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
