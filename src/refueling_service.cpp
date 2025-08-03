// refueling_service.cpp
#include "refueling_service.h"
#include <loger.h>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

namespace TaxiSystem {

    RefuelingService::RefuelingService(std::shared_ptr<ICarPool> car_pool)
        : car_pool(car_pool) {
        startService();
    }

    RefuelingService::~RefuelingService() {
        stopService();
    }

    void RefuelingService::run() {
        while (running) {
            auto cars = car_pool->getAllCars();
            for (auto& car : cars) {
                if (!running) break; // Быстрый выход
                if (car->status == CarStatus::Refueling) {
                    LOG_INFO_MSG(std::format("Refueling car {}...",car->id));

                    for (int i = 0; i < 300 && running; ++i) { // 300 итераций по 0.1 сек = 30 сек
                        std::this_thread::sleep_for(100ms);
                        if (!running) break;
                    }
                    if(running){
                        car->fuel_level = 100.0;
                        car->status = CarStatus::Ready;
                        LOG_INFO_MSG(std::format("Car {} refueled and ready.", car->id));
                    }
                    else{
                        LOG_INFO_MSG("RefuelingService: graceful shutdown");
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void RefuelingService::startService() {
        running = true;
        service_thread = std::thread([this]() { this->run(); });
    }

    void RefuelingService::stopService() {
        running = false;
        if (service_thread.joinable()) {
            service_thread.join();
        }
    }

}
