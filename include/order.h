// order.h
#pragma once

#include <chrono>
#include <memory>

namespace TaxiSystem {

    struct Order {
        int id;
        double distance_km;
        double completed_distance_km = 0.0;
        std::chrono::system_clock::time_point last_update;
        bool is_completed = false;
        int assigned_car_id = -1;
    };

}
