#pragma once
#include "Todo.h"
#include <unordered_map>

class Store
{
public:
    void insert(std::int64_t id, const TodoProperties& properties);
    void update(std::int64_t id, const TodoProperties& properties);
    TodoProperties get(std::int64_t id) const;
    void remove(std::int64_t id);
    bool checkId(std::int64_t id);
    std::vector<std::int64_t> query(const TodoProperty& property) const;
    std::vector<std::int64_t> rangeQuery(double minTimeStamp, double maxTimeStamp) const;

    Store createChild() const;
    void commit();
private:
    std::unordered_map<std::int64_t, Todo> todos;
    Store* parent{nullptr};
};



