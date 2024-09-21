/**
 * Coding practice of async thread pool
 */

#include <chrono>
#include <iostream>
#include <vector>
#include <future>

#include "utils.h"
#include "ThreadPool.h"

int main(int argc, char *argv[]) {

    const size_t numThreads{static_cast<size_t>(std::stoi(argv[1]))};
    const size_t numTask{static_cast<size_t>(std::stoi(argv[2]))};

    typedef int varType1;
    typedef float varType2;

    // deduce the return type of the future, and use it to define the vector of future
    typedef decltype(addNumbers<varType1>) F1;
    typedef std::invoke_result_t<F1, varType1, varType1> T1;
    std::vector<std::future<T1>> futures1;

    typedef decltype(printNumber<varType2>) F2;
    typedef std::invoke_result_t<F2, varType2> T2;
    std::vector<std::future<T2>> futures2;

    {
        // put the thread pool in a local scope such that all the thread will join when out of the scope
        ThreadPool pool(numThreads);
        for (size_t i = 0; i < numTask; ++i) {
            auto var1 = static_cast<varType1>(i);
            futures1.emplace_back(pool.addTask(addNumbers<varType1>, var1, var1));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            auto var2 = static_cast<varType2>(i);
            futures2.emplace_back(pool.addTask(printNumber<varType2>, var2));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // retrieve the results from futures with returned value
    for (auto & future : futures1) {
        std::cout << "Result 1 = " << future.get() << '\n';
    }

    // Not allowed; the compiler is so smart that it can tell the return of feature2 is void
    // for (auto & future : futures2) {
    //     std::cout << "Result: " << future.get() << std::endl;
    // }

    return EXIT_SUCCESS;
}