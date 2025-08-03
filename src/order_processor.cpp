// order_processor.cpp
#include "order_processor.h"
#include <iostream>
#include <chrono>
#include <random>
#include <format>

#include <unistd.h>
#include <sys/syscall.h>
using namespace std::chrono_literals;

namespace TaxiSystem {

    OrderProcessor::OrderProcessor(std::shared_ptr<IOrderDispatcher> dispatcher,
                 std::shared_ptr<ICarPool> car_pool,
                 int num_workers,
                 double average_speed_kmh,
                 double fuel_consumption_per_km)
                 :dispatcher(dispatcher), car_pool(car_pool), average_speed_kmh_(average_speed_kmh), fuel_consumption_per_km_(fuel_consumption_per_km)
    {
        startWorkers(num_workers);
    }


    OrderProcessor::~OrderProcessor() {
        stopWorkers();
    }

    // цикл воркера. Работает, пока не получит сигнал закрыться.
    void OrderProcessor::processOrders(int tid) {
        thread_local int tid__ = tid;
        //this->tid_ = tid;
        while (running) {
            // 1. Сначала обрабатываем движущиеся машины
            if (processActiveOrders(tid__)) {
                continue; // Если нашли активный заказ - сразу в новый цикл
            }
        
            // 2. Если активных нет - берем новый заказ
            if (dispatcher->hasOrders()) {
                processNewOrder(tid__);
            }
        
            // 3. Короткая пауза при полном отсутствии работы
            std::this_thread::sleep_for(10ms);
        }
    }

    void OrderProcessor::startWorkers(int num_workers) {
        running = true;
        for (int i = 0; i < num_workers; ++i) {
            workers.emplace_back([this, i]() { /*this->tid_ = i;*/ this->processOrders(i); });
        }
    }

    void OrderProcessor::stopWorkers() {
        running = false;
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

void OrderProcessor::processNewOrder(int tid) {

    auto order = dispatcher->getNextOrder();
    if (!order) return;

    auto car = car_pool->getAvailableCar();

    if (car) {
        std::lock_guard<std::mutex> lock(car->mutex);
        dispatcher->addActiveOrder(order);
        
        order->assigned_car_id = car->id;
        order->last_update = std::chrono::system_clock::now();
        car->current_order_id = order->id;
        car->trip_start_time = order->last_update;  // Фиксируем время начала поездки
        car->ts_ = order->last_update;
        LOG_INFO_MSG(std::format("[{}] Order {} assigned to car {}, distance: {:.2f} km", 
                  tid, order->id, car->id, order->distance_km));
    } else {
        LOG_INFO_MSG(std::format("[{}] No cars. Requeue order {}", tid, order->id));
        dispatcher->addOrder(order); // Возвращаем в общую очередь        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool OrderProcessor::processActiveOrders(int tid) {
    auto cars = car_pool->getAllCars();
    
    for (auto& car : cars) {
        // Быстрая проверка статуса без блокировки
        if (car->status != CarStatus::Moving || car->current_order_id == -1) continue;
        //log_.out("processActiveOrders: moving car with order found");
        // Получаем текущее время один раз
        const auto now = std::chrono::system_clock::now();
        
        // Блокируем машину для проверки
        std::unique_lock lock(car->mutex, std::try_to_lock);
        if (!lock.owns_lock()) continue; // Не смогли заблокировать - пропускаем
        //log_.out(std::format("[{}] car {} is locked", tid, car->id));

        // Проверяем время поездки
        const auto trip_duration = now - car->trip_start_time;
        const double trip_duration_sec = std::chrono::duration<double>(trip_duration).count();
        
        // Получаем заказ (потокобезопасно через dispatcher)
        auto order = dispatcher->getOrderById(car->current_order_id);
        if (!order) continue;
        //log_.out(std::format("[{}] order {} is loaded", tid, order->id));
        
        // Рассчитываем оставшееся время поездки
        const double required_time = (order->distance_km / average_speed_kmh_) * 3600;
        const bool is_trip_completed = (trip_duration_sec >= required_time);
        const bool needs_update = (now - car->ts_ >= std::chrono::seconds(3));
        
        if (is_trip_completed || needs_update) {
            LOG_INFO_MSG(std::format("[{}] car {}, order {}, dur = {:.2f}, req = {:.2f}", tid, car->id, car->current_order_id, trip_duration_sec, required_time));
            updateCarAndOrder(car, order, trip_duration_sec, is_trip_completed);
            return true;
        }
    }
    return false;
}

    void OrderProcessor::updateCarAndOrder(
        std::shared_ptr<Car> car, 
        std::shared_ptr<Order> order,
        double trip_duration_sec,
        bool is_trip_completed) 
    {
        // Рассчитываем пройденное расстояние
        const double distance_covered = (average_speed_kmh_ / 3600.0) * trip_duration_sec;
    
        // Обновляем показатели машины
        car->mileage_km += distance_covered;
        car->fuel_level -= distance_covered * fuel_consumption_per_km_;
        car->ts_ = std::chrono::system_clock::now();
        car->current_ord_km = distance_covered;
    
        // Обновляем заказ
        order->completed_distance_km = distance_covered;
        order->last_update = car->ts_;
    
        if (is_trip_completed) {
            // Завершаем заказ
            order->is_completed = true;
            dispatcher->completeOrder(order->id);
        
            // Освобождаем машину
            car->current_order_id = -1;
            car->current_ord_km = 0.0;
        
            // Проверяем топливо
            if (car->fuel_level < 50.0) {
                car->status = CarStatus::Refueling;
            } else {
                car->status = CarStatus::Ready;
            }
            LOG_INFO_MSG(std::format("!* Order {} finished, fuel: {:.2f}%", order->id, car->fuel_level.load()));
        }
    }

    void OrderProcessor::updateCarMetrics(std::shared_ptr<Car> car) {
        const auto now = std::chrono::system_clock::now();
        const double elapsed_hours = 
            std::chrono::duration<double>(now - car->ts_).count() / 3600.0;
    
        const double distance = average_speed_kmh_ * elapsed_hours;
    
        car->mileage_km += distance;
        car->current_ord_km += distance;
        car->fuel_level -= distance * fuel_consumption_per_km_;
        car->ts_ = now;
    }
}
