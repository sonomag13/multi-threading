#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template<class T>
class HighEfficiencyQueue {
public:
    HighEfficiencyQueue() = default;
    ~HighEfficiencyQueue() = default;

    void put(T value) {
        /*
         * Put a value in the queue
         */

        std::lock_guard lock(_mtx);
        this->_que.emplace(std::move(value));
        this->_cv.notify_one();
    }

    T get() {
        /*
         * Get a data point from the front of the queue
         */

        std::unique_lock lock(_mtx);

        this->_cv.wait(lock, [this]() { return !this->_que.empty(); });
        auto value = std::move(this->_que.front());
        _que.pop();

        return value;
    }

private:
    std::queue<T> _que;
    std::mutex _mtx;
    std::condition_variable _cv;
};