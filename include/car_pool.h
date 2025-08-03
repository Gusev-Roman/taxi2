// car_pool.h
#pragma once

#include "interfaces.h"
#include <vector>
#include <mutex>

namespace TaxiSystem {

    class CarPool : public ICarPool {
    public:
        explicit CarPool(int num_cars);
        ~CarPool() override;
        std::shared_ptr<Car> getAvailableCar() override;
        std::vector<std::shared_ptr<Car>> getAllCars() override;
        std::shared_ptr<Car> getCar(int id) override;
        void printFinalReport() const;

    private:
        std::vector<std::shared_ptr<Car>> cars;
        mutable std::mutex cars_mutex;
        std::string statusToString(CarStatus status) const;
    };

}
