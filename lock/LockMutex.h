#pragma once

#include <chrono>
#include <mutex>
#include <thread>

class LockMutex {
public:
    LockMutex() = default;
    ~LockMutex() = default;

    void addNumWithoutMutex() {
        auto start = std::chrono::high_resolution_clock::now();

        std::thread t1([this]() {
            _addNumWithoutMutex();
        });
        std::thread t2([this]() {
            _addNumWithoutMutex();
        });
        t1.join();
        t2.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "Without mutex" << '\n';
        std::cout << "\tsum = " << _sum << '\n';
        std::cout << "\truntime = " << duration.count() << " usec" << '\n';
    }

    void addNumWithMutex() {
        auto start = std::chrono::high_resolution_clock::now();
        std::thread t1([this]() {
            _addNumWithMutex();
        });
        std::thread t2([this]() {
            _addNumWithMutex();
        });
        t1.join();
        t2.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "With mutex" << '\n';
        std::cout << "\tsum = " << _sum << '\n';
        std::cout << "\truntime = " << duration.count() << " usec" << '\n';
    }

    void addNumWithAtomic() {
        auto start = std::chrono::high_resolution_clock::now();
        std::thread t1([this]() {
            _addNumWithAtomic();
        });
        std::thread t2([this]() {
            _addNumWithAtomic();
        });
        t1.join();
        t2.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "With atomic" << '\n';
        std::cout << "\tsum = " << _atomicSum << '\n';
        std::cout << "\truntime = " << duration.count() << " usec" << '\n';
    }

    void addNumWithLockGuard() {
        auto start = std::chrono::high_resolution_clock::now();
        std::thread t1([this]() {
            _addNumWithMutex();
        });
        std::thread t2([this]() {
            _addNumWithMutex();
        });
        t1.join();
        t2.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "With lock guard" << '\n';
        std::cout << "\tsum = " << _sum << '\n';
        std::cout << "\truntime = " << duration.count() << " usec" << '\n';
    }

    void reset() {
        _sum = 0;
    }

private:
    std::mutex _mtx;
    int _sum{0};
    std::atomic<int> _atomicSum{0};
    int _target{10000};

    void _addNumWithoutMutex() {
        // add to sum without mutex

        for (int i = 0; i < _target; ++i) {
            ++_sum;
        }
    }

    void _addNumWithAtomic() {
        // add to sum without mutex

        for (int i = 0; i < _target; ++i) {
            ++_atomicSum;
        }
    }

    void _addNumWithMutex() {
        // add to sum with mutex

        for (int i = 0; i < _target; ++i) {
            _mtx.lock();
            ++_sum;
            _mtx.unlock();
        }
    }

    void _addNumWithLockGuard() {
        // add to sum without mutex

        for (int i = 0; i < _target; ++i) {
            std::lock_guard<std::mutex> lg(_mtx);
            ++_sum;
        }
    }

};
