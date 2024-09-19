#include <chrono>
#include <iostream>
#include <vector>
#include <future>

#include "ThreadPool.h"

// Function to be used as a task
int printNumber(int val) {

    std::thread::id threadId = std::this_thread::get_id();
    std::cout << threadId << ": value = " << val << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return val;
}

int main() {

    std::vector<std::future<int>> futures;

    // Add tasks to the ThreadPool and store the futures in the vector
    {
        const size_t numThreads = 10;
        ThreadPool pool(numThreads);

        const size_t numTask{50};
        for (int i = 0; i < numTask; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            futures.emplace_back(pool.addTask(printNumber, i));
        }
    }

    // Retrieve the results
    for (auto &fut : futures) {
        std::cout << "Result: " << fut.get() << std::endl;
    }

    return 0;
}