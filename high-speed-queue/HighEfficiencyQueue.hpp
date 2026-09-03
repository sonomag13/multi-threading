#pragma once

#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>

template<class T, std::size_t Capacity>
class HighEfficiencyQueue {
    static_assert(Capacity > 0, "Capacity must be greater than 0");

public:
    HighEfficiencyQueue() = default;
    ~HighEfficiencyQueue() = default;

    void close() {

        {
            std::lock_guard<std::mutex> lock(this->_mtx);
            std::cout << "Queue is closed";
            this->_shutdown = true;
        }

        _cv_not_empty.notify_all();
        _cv_not_full.notify_all();
    }

    bool put(const T& value) {
        /*
         * Put a value in the queue
         */

        std::unique_lock lock(this->_mtx);
        if (this->_que.size() == Capacity) {
            std::cout << "Queue is full\n";
        }
        this->_cv_not_full.wait(lock, [this] () {
                std::cout <<  "queue size = " << this->_que.size() << '\n';
                return this->_shutdown || this->_que.size() < Capacity;
            }
        );

        if (this->_shutdown) {
            std::cerr << "shut down\n";
            return false;
        }

        this->_que.emplace(value);
        lock.unlock();
        this->_cv_not_empty.notify_one();
        return true;
    }

    bool put(T&& value) {
        /*
         * Put a value in the queue
         */

        std::unique_lock lock(this->_mtx);
        if (this->_que.size() == Capacity) {
            std::cout << "Queue is full\n";
        }
        this->_cv_not_full.wait(lock, [this] () {
                std::cout <<  "queue size = " << this->_que.size() << '\n';
                return this->_shutdown || this->_que.size() < Capacity;
            }
        );

        if (this->_shutdown) {
            std::cerr << "shut down\n";
            return false;
        }

        this->_que.emplace(std::move(value));
        lock.unlock();

        this->_cv_not_empty.notify_one();
        return true;
    }

    std::optional<T> get() {
        /*
         * Get a data point from the front of the queue
         */

        std::unique_lock lock(this->_mtx);
        if (_que.empty()) {
            std::cout << "Queue is empty\n";
        }
        this->_cv_not_empty.wait(lock, [this]() {
                std::cout <<  "queue size = " << this->_que.size() << '\n';
                return this->_shutdown || !this->_que.empty();
            }
        );

        if (this->_que.empty()) {
            std::cerr << "shut down\n";
            return std::nullopt;
        }

        auto value = std::move(this->_que.front());
        _que.pop();
        lock.unlock();

        this->_cv_not_full.notify_one();

        return value;
    }

private:
    bool _shutdown{false};
    std::queue<T> _que;
    std::mutex _mtx;
    std::condition_variable _cv_not_empty;
    std::condition_variable _cv_not_full;
};
