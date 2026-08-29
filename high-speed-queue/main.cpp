#include <chrono>
#include <iostream>
#include <thread>

#include "IMUData.hpp"
#include "HighEfficiencyQueue.hpp"

int main() {

    HighEfficiencyQueue<IMUData> queImuData;
    IMUData imuData;

    std::thread threadGet(
    [&queImuData] {
        auto imuData = queImuData.get();
        std::cout << imuData.timestamp << '\n';
        }
    );

    std::thread threadPut(
        [&queImuData, &imuData] {
            queImuData.put(imuData);
        }
    );

    // std::this_thread::sleep_for(std::chrono::seconds(1));

    threadPut.join();
    threadGet.join();

    return EXIT_SUCCESS;
}
