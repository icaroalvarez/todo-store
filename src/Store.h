#pragma once
#include "Todo.h"
#include <memory>
#include <unordered_set>

class Store: public std::enable_shared_from_this<Store>
{
public:
    virtual void insert(std::int64_t id, const TodoProperties& properties) = 0;
    virtual void update(std::int64_t id, const TodoProperties& properties) = 0;
    virtual TodoProperties get(std::int64_t id) const = 0;
    virtual void remove(std::int64_t id) = 0;
    virtual bool checkId(std::int64_t id) const = 0;
    virtual std::unordered_set<std::int64_t> query(const TodoProperty& property) const = 0;
    virtual std::unordered_set<std::int64_t> rangeQuery(double minTimeStamp, double maxTimeStamp) const = 0;
    virtual std::unique_ptr<Store> createChild() = 0;
    virtual void commit() = 0;
};



