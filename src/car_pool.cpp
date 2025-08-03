// car_pool.cpp
#include <iostream>
#include <format>
#include "car_pool.h"
#include "loger.h"

namespace TaxiSystem {

    CarPool::CarPool(int num_cars) {
        std::lock_guard<std::mutex> lock(cars_mutex);
        for (int i = 0; i < num_cars; ++i) {
            auto car = std::make_shared<Car>(i+1);
            car->id = i + 1;
            cars.push_back(car);
        }
    }

    CarPool::~CarPool(){
        LOG_INFO_MSG("CarPool is destroyed");
    }

    std::shared_ptr<Car> CarPool::getAvailableCar() {
        std::lock_guard<std::mutex> lock(cars_mutex);
        for (auto& car : cars) {
            if (car->status == CarStatus::Ready) {
                car->status = CarStatus::Moving;
                car->ts_ = std::chrono::system_clock::now();    // time of start!
                return car;
            }
        }
        return nullptr; 
    }

    std::vector<std::shared_ptr<Car>> CarPool::getAllCars() {
        std::lock_guard<std::mutex> lock(cars_mutex);
        return cars;
    }

    std::shared_ptr<Car> CarPool::getCar(int id) {
        std::lock_guard<std::mutex> lock(cars_mutex);
        if (id > 0 && id <= static_cast<int>(cars.size())) {
            return cars[id - 1];
        }
        return nullptr;
    }

    void CarPool::printFinalReport() const {
        std::lock_guard<std::mutex> lock(cars_mutex);
    
        LOG_INFO_MSG(std::format("\n=== FINAL CARS REPORT ==="));
        for (const auto& car : cars) {
            std::lock_guard<std::mutex> car_lock(car->mutex);
            LOG_INFO_MSG(std::format("Car {} | Status: {} | Mileage: {:.2f} km | Fuel: {:.2f}% | Current order: {}",
                 car->id,
                 statusToString(car->status),
                 car->mileage_km.load(),
                 car->fuel_level.load(),
                 (car->current_order_id != -1 ? std::to_string(car->current_order_id) : "None")
                ));
        }
        LOG_INFO_MSG("========================\n");
    }

    std::string CarPool::statusToString(CarStatus status) const {
        switch(status) {
            case CarStatus::Ready: return "Ready";
            case CarStatus::Moving: return "Moving";
            case CarStatus::Refueling: return "Refueling";
            case CarStatus::Alert: return "Alert";
            default: return "Unknown";
        }
    }
}
