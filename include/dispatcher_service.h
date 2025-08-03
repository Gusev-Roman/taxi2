// dispatcher_service.h
#pragma once

#include "interfaces.h"
#include <atomic>
#include <thread>

namespace TaxiSystem {

    class DispatcherService {
    public:
        DispatcherService(std::shared_ptr<IOrderDispatcher> dispatcher, 
                         std::shared_ptr<ICarPool> car_pool);
        ~DispatcherService();

        void run();
        void stop();

    private:
        void startService();
        void stopService();
        void createInitialOrders(int num_orders);
        void createRandomOrder();

        std::shared_ptr<IOrderDispatcher> dispatcher;
        std::shared_ptr<ICarPool> car_pool;
        std::thread dispatcher_thread;
        std::atomic<bool> running{false};
        // счетчик заказов
        std::atomic<int> order_id{0};
    };

}
