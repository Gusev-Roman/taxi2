// refueling_service.h
#pragma once

#include "interfaces.h"
#include <atomic>
#include <thread>

namespace TaxiSystem {

    class RefuelingService {
    public:
        explicit RefuelingService(std::shared_ptr<ICarPool> car_pool);
        ~RefuelingService();

        void run();
        void stop();

    private:
        void startService();
        void stopService();

        std::shared_ptr<ICarPool> car_pool;
        std::thread service_thread;
        std::atomic<bool> running{false};
    };

}
