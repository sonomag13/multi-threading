#include <chrono>
#include <iostream>
#include <thread>

#include "IMUData.hpp"
#include "HighEfficiencyQueue.hpp"
#include "utils.hpp"

constexpr size_t QUEUE_CAPACITY{10};

int main() {

    HighEfficiencyQueue<IMUData, QUEUE_CAPACITY> queImuData;

    std::thread threadGet(
    [&queImuData] {
            getData(queImuData, std::chrono::milliseconds(1));
        }
    );

    std::thread threadPut(
        [&queImuData] {
            putData(queImuData, std::chrono::milliseconds(100));
        }
    );

    threadPut.join();
    threadGet.join();

    return EXIT_SUCCESS;
}
