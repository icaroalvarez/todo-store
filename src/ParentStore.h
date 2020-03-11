#pragma once
#include "Todo.h"
#include <unordered_map>
#include <map>
#include <list>
#include <memory>
#include "Store.h"
#include "StringPropertyIds.h"
#include "DoublePropertyIds.h"

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
     * titleIds will keep track of all ids with the same title in order
     * to improve the querying by title.
     */
    StringPropertyIds titleIds;
    /**
     * timestampsIds will keep track of all ids related to timestamps values
     * in order to improve the timestamp range query feature.
     */
    DoublePropertyIds timestampIds;
};



