// dispatcher_service.cpp
#include "dispatcher_service.h"
#include <random>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;


namespace TaxiSystem {

    DispatcherService::DispatcherService(std::shared_ptr<IOrderDispatcher> dispatcher, 
                                       std::shared_ptr<ICarPool> car_pool)
        : dispatcher(dispatcher), car_pool(car_pool) {
        startService();
    }

    DispatcherService::~DispatcherService() {
        std::cout << "DispatcherService destructor...\n";
        if(running){
             stopService();
             std::cout << "destructor done!\n";
        }
        car_pool->printFinalReport(); 
    }

    void DispatcherService::run() {
        auto cars = car_pool->getAllCars();

        createInitialOrders(cars.size() * 2);

        while (running) {
            createRandomOrder();

            for (int i = 0; i < 30 && running; ++i) {
                std::this_thread::sleep_for(1s);
            }            
        }
    }
    void DispatcherService::stop() {
        stopService();
    }

    void DispatcherService::startService() {
        running = true;
        dispatcher_thread = std::thread([this]() { this->run(); });
    }

    void DispatcherService::stopService() {
        running = false;
        if (dispatcher_thread.joinable()) {
            std::cout << "try to join...\n";
            dispatcher_thread.join();
            std::cout << "join done!\n";
        }
    }

    void DispatcherService::createInitialOrders(int num_orders) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(1.0, 3.0);   // уменьшим дистанцию чтоб не ждать долго

        for (int i = 0; i < num_orders; ++i) {
            auto order = std::make_shared<Order>();
            order->id = ++order_id;
            order->distance_km = dist(gen);
            dispatcher->addOrder(order);
            std::cout << "*New order created: ID " << order->id 
                  << ", distance: " << order->distance_km << " km\n";

        }
    }

    void DispatcherService::createRandomOrder() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(1.0, 3.0);    // тоже уменьшили дистанцию

        auto order = std::make_shared<Order>();
        order->id = ++order_id;
        order->distance_km = dist(gen);
        dispatcher->addOrder(order);

        std::cout << "New order created: ID " << order->id << ", distance: " << order->distance_km << " km\n";
    }

}
