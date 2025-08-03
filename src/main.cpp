#include "interfaces.h"
#include "order_dispatcher.h"
#include "car_pool.h"
#include "order_processor.h"
#include "refueling_service.h"
#include "dispatcher_service.h"
#include "loger.h"
#include <iostream>
#include <memory>
#include <thread>

using namespace TaxiSystem;

int main() {
    const int num_cars = 40;
    const int num_workers = 4;

    auto dispatcher = std::make_shared<OrderDispatcher>();
    std::this_thread::sleep_for(std::chrono::seconds(1));    
    auto car_pool = std::make_shared<CarPool>(num_cars);

    auto order_processor = std::make_unique<OrderProcessor>(dispatcher, car_pool, num_workers, 60.0, 0.5);

    auto refueling_service = std::make_unique<RefuelingService>(car_pool);
    auto dispatcher_service = std::make_unique<DispatcherService>(dispatcher, car_pool);

    LOG_INFO_MSG(std::format("main(): Taxi system started with {} cars and {} workers", num_cars, num_workers));

    std::this_thread::sleep_for(std::chrono::minutes(5));

    LOG_INFO_MSG("main(): Shutting down dispatcher...");
    dispatcher_service->stop();

    // даем время доехать
    std::this_thread::sleep_for(std::chrono::minutes(1));
    LOG_INFO_MSG("main(): Shutting down the system...");
    return 0;
}
