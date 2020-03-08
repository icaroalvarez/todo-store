#pragma once
#include "Todo.h"
#include <unordered_map>
#include <map>
#include <list>
#include <memory>
#include "Store.h"

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
    std::unordered_map<std::string, std::unordered_set<std::int64_t>> titleIds;
    std::map<double, std::unordered_set<std::int64_t>> timestampIds;
    std::weak_ptr<Store> parent;
    std::list<std::shared_ptr<Store>> children;
};



