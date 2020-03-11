#pragma once
#include "Todo.h"
#include <unordered_map>
#include <map>
#include <list>
#include <memory>
#include "Store.h"
#include "StringPropertyIds.h"

class ParentStore: public Store
{
public:
    void insert(std::int64_t id, const TodoProperties& properties) override;
    void update(std::int64_t id, const TodoProperties& properties) override;
    TodoProperties get(std::int64_t id) const override;
    void remove(std::int64_t id) override;
    bool checkId(std::int64_t id) override;
    std::unordered_set<std::int64_t> query(const TodoProperty& property) const override;
    std::unordered_set<std::int64_t> rangeQuery(double minTimeStamp, double maxTimeStamp) const override;
    std::shared_ptr<Store> createChild() override;
    void commit() override;
private:
    std::unordered_map<std::int64_t, Todo> todos;
    /**
     * titleIds is an associative container that will keep track of all ids with the same title in order
     * to improve the query by title feature. Unordered container were chosen because it performance better
     * than the ordered ones and there is no need to keep the values ordered.
     */
    //std::unordered_map<std::string, std::unordered_set<std::int64_t>> titleIds;
    StringPropertyIds titleIds;
    /**
     * timestampsIds is an associative container that will keep track of all ids related to timestamps values
     * in order to improve the timestamp range query feature. A sorted container is more convenient than an
     * unordered one to improve the performance while searching ranges (from min value to mas value).
     * A multi map is more convenient because more than one id can have the same timestamp and makes it faster
     * to create the output set when calling rangeQuery.
     */
    std::multimap<double, std::int64_t> timestampIds;
    std::list<std::shared_ptr<Store>> children;
};



