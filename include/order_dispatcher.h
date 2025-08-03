// order_dispatcher.h
#pragma once

#include "interfaces.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

namespace TaxiSystem {

    class OrderDispatcher : public IOrderDispatcher {
    public:
        ~OrderDispatcher();
        void addOrder(std::shared_ptr<Order> order) override;
        std::shared_ptr<Order> getNextOrder() override;
        bool hasOrders() const override;
        void addActiveOrder(std::shared_ptr<Order> order) override;

        std::shared_ptr<Order> getOrderById(int id) const {
            std::lock_guard lock(orders_mutex);
            if (active_orders.count(id)) return active_orders.at(id);
            if (completed_orders.count(id)) return completed_orders.at(id);
            return nullptr;
        }
    
    void completeOrder(int order_id) {
        std::lock_guard lock(orders_mutex);
        if (active_orders.count(order_id)) {
            completed_orders[order_id] = active_orders[order_id];
            active_orders.erase(order_id);
        }
    }        

    private:
        std::queue<std::shared_ptr<Order>> orders;
        mutable std::mutex orders_mutex;
        std::condition_variable cv;

        std::unordered_map<int, std::shared_ptr<Order>> active_orders;
        std::unordered_map<int, std::shared_ptr<Order>> completed_orders;        
    };

}

