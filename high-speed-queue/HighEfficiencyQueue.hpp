#pragma once

#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>

template<class T, std::size_t Capacity>
class HighEfficiencyQueue {
    static_assert(Capacity > 0, "Capacity must be greater than 0");

public:
    HighEfficiencyQueue() = default;
    ~HighEfficiencyQueue() = default;

    void put(const T& value) {
        /*
         * Put a value in the queue
         */

        std::cout << "lvalue is passed in" << '\n';
        {
            std::unique_lock lock(this->_mtx);
            this->_cv_not_full.wait(lock, [this] () {return this->_que.size() < Capacity;});
            this->_que.emplace(value);
        }

        this->_cv_not_empty.notify_one();
    }

    void put(T&& value) {
        /*
         * Put a value in the queue
         */

        std::cout << "rvalue is passed in" << '\n';
        {
            std::unique_lock lock(this->_mtx);
            this->_cv_not_full.wait(lock, [this] () {return this->_que.size() < Capacity;});
            this->_que.emplace(std::move(value));
        }

        this->_cv_not_empty.notify_one();
    }

    T get() {
        /*
         * Get a data point from the front of the queue
         */

        std::unique_lock lock(this->_mtx);

        if (_que.empty()) {
            std::cout << "Queue is empty; waiting for data\n";
        }

        this->_cv_not_empty.wait(lock, [this]() { return !this->_que.empty(); });
        auto value = std::move(this->_que.front());
        _que.pop();
        lock.unlock();

        this->_cv_not_full.notify_one();

        return value;
    }

private:
    std::queue<T> _que;
    std::mutex _mtx;
    std::condition_variable _cv_not_empty;
    std::condition_variable _cv_not_full;
};
