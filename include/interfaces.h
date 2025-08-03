// interfaces.h
#pragma once

#include <memory>
#include <vector>
#include "order.h"
#include "car.h"

namespace TaxiSystem {

    class IOrderDispatcher {
    public:
        virtual ~IOrderDispatcher() = default;
        virtual void addOrder(std::shared_ptr<Order> order) = 0;
        virtual std::shared_ptr<Order> getNextOrder() = 0;
        virtual bool hasOrders() const = 0;
        virtual std::shared_ptr<Order> getOrderById(int id) const = 0;
        virtual void completeOrder(int order_id) = 0;
        virtual void addActiveOrder(std::shared_ptr<Order> order) = 0;
    };

    class ICarPool {
    public:
        virtual ~ICarPool() = default;
        virtual std::shared_ptr<Car> getAvailableCar() = 0;
        virtual std::vector<std::shared_ptr<Car>> getAllCars() = 0;
        virtual std::shared_ptr<Car> getCar(int id) = 0;
        virtual void printFinalReport() const = 0;
    };

    class IOrderProcessor {
    public:
        virtual ~IOrderProcessor() = default;
        virtual void processOrders(int tid) = 0;
    };

}
