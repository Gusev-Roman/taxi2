// order_dispatcher.cpp
#include "order_dispatcher.h"

namespace TaxiSystem {

    OrderDispatcher::~OrderDispatcher(){
        std::cout << "OrderDispatcher destroyed\n";
    }

    void OrderDispatcher::addOrder(std::shared_ptr<Order> order) {
        std::lock_guard<std::mutex> lock(orders_mutex);
        orders.push(order);
        cv.notify_one();        // будим любой из воркеров
    }

    std::shared_ptr<Order> OrderDispatcher::getNextOrder() {
        std::unique_lock<std::mutex> lock(orders_mutex);
        cv.wait(lock, [this] { return !orders.empty(); });
        auto order = orders.front();
        orders.pop();
        return order;
    }

    bool OrderDispatcher::hasOrders() const {
        std::lock_guard<std::mutex> lock(orders_mutex);
        return !orders.empty();
    }

    void OrderDispatcher::addActiveOrder(std::shared_ptr<Order> order) {
        std::lock_guard lock(orders_mutex);
        active_orders[order->id] = order;
    }
}
