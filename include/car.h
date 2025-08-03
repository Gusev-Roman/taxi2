// car.h
#pragma once

#include <atomic>
#include <mutex>
#include <iostream>
#include <format>
#include "loger.h"

namespace TaxiSystem {

    enum class CarStatus {
        Ready,
        Moving,
        Refueling,
        Alert
    };

    struct Car {
        Car(int carid) {
            LOG_INFO_MSG(std::format("Car {} mutex initialized\n", carid));
        }
        ~Car() {
            //LOG_INFO_MSG(std::format("Car {} mutex destroyed\n", id));
        }        
        int id;
        std::chrono::system_clock::time_point ts_;
        std::chrono::system_clock::time_point trip_start_time;
        std::mutex mutex;
        std::atomic<double> current_ord_km{0.0};
        std::atomic<CarStatus> status{CarStatus::Ready};
        std::atomic<double> fuel_level{100.0};
        std::atomic<double> mileage_km{0.0};
        int current_order_id = -1;
    };
}
