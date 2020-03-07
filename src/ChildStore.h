#pragma once
#include "Todo.h"
#include <unordered_map>
#include <map>
#include <list>
#include <memory>
#include <set>
#include "Store.h"

class ChildStore: public Store
{
public:
    explicit ChildStore(const std::shared_ptr<Store>& parent);
    void insert(std::int64_t id, const TodoProperties& properties) override;
    void update(std::int64_t id, const TodoProperties& properties) override;
    TodoProperties get(std::int64_t id) const override;
    void remove(std::int64_t id) override;
    bool checkId(std::int64_t id) override;
    std::vector<std::int64_t> query(const TodoProperty& property) const override;
    std::vector<std::int64_t> rangeQuery(double minTimeStamp, double maxTimeStamp) const override;

    std::shared_ptr<Store> createChild();
    void commit();
private:
    std::unordered_map<std::int64_t, TodoProperties> todosToBeInserted;
    std::unordered_map<std::int64_t, TodoProperties> propertiesToBeUpdated;
    std::set<std::int64_t> idsToBeRemoved;
    std::weak_ptr<Store> parent;
};



