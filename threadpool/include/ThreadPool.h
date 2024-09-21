#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads): _stop(false) {
        /**
         * Constructor
         */

        if (numThreads == 0) {
            throw std::invalid_argument("[Error] Number of threads is 0");
        }

        for (size_t i = 0; i < numThreads; ++i) {
            _workers.emplace_back(&ThreadPool::_run_task, this);
        }
    }

    ~ThreadPool() {
        /**
         * Destructor
         */
        {
            std::unique_lock lock(_mtx);
            _stop = true;
        }

        _cv.notify_all();

        for (auto & worker : _workers) {
            worker.join();
        }
    }

    template<class F, class ... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args ...>> {
        /**
         * Add a task to the queue
         */

        typedef std::invoke_result_t<F, Args...> return_type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res{task->get_future()};

        {
            std::unique_lock lock(_mtx);
            _tasks.emplace([task]() {
                (*task)();
            });

            _cv.notify_one();
        }

        return res;
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    bool _stop;  // Flag to indicate whether to stop
    std::vector<std::thread> _workers;  // Vector of worker threads
    std::queue<std::function<void()>> _tasks;  // Queue of tasks

    void _run_task() {
        /**
          * Worker thread function that processes tasks from the queue.
          */

        std::function<void()> task;

        while (true) {
            {
                std::unique_lock lock(_mtx);
                _cv.wait(lock, [this] () {return !_tasks.empty() || _stop;});

                if (_stop && _tasks.empty()) {
                    return;
                }

                task = std::move(_tasks.front());
                _tasks.pop();
            }

            task();
        }
    }

};
