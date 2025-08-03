// order_processor.h
#pragma once

#include "interfaces.h"
#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include "loger.h"

namespace TaxiSystem {

    class OrderProcessor : public IOrderProcessor {
    public:
        OrderProcessor(std::shared_ptr<IOrderDispatcher> dispatcher,
                 std::shared_ptr<ICarPool> car_pool,
                 int num_workers = 8,
                 double average_speed_kmh = 60.0,
                 double fuel_consumption_per_km = 0.2); // 0.2% на 1 км (100%/500км)                      

        ~OrderProcessor();

        void processOrders(int tid) override;

    private:
        void startWorkers(int num_workers);
        void stopWorkers();
        void processNewOrder(int id);
        bool processActiveOrders(int id);
        //void updateCarAndOrder(std::shared_ptr<Car> car);
        void updateCarAndOrder(std::shared_ptr<Car> car, std::shared_ptr<Order> order, double trip_duration_sec, bool is_trip_completed);


        void completeOrder(std::shared_ptr<Car> car, const Order& order);
        void updateCarMetrics(std::shared_ptr<Car> car);

        std::shared_ptr<IOrderDispatcher> dispatcher;
        std::shared_ptr<ICarPool> car_pool;
        std::vector<std::thread> workers;
        std::atomic<bool> running{false};
        //Logger log_;
        mutable std::mutex orders_mutex;
        thread_local static int tid_;  // Идентификатор потока 

        const double average_speed_kmh_;            // инициализируются конструктором
        const double fuel_consumption_per_km_;
    };
}
