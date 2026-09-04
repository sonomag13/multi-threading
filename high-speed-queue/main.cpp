#include <chrono>
#include <iostream>
#include <thread>

#include "IMUData.hpp"
#include "HighEfficiencyQueue.hpp"
#include "utils.hpp"

constexpr size_t QUEUE_CAPACITY{10};

int main() {

    HighEfficiencyQueue<IMUData, QUEUE_CAPACITY> queImuData;

    std::thread threadConsumer1(
        [&queImuData] {
            getData(queImuData, std::chrono::milliseconds(50));
        }
    );
    std::thread threadConsumer2(
        [&queImuData] {
            getData(queImuData, std::chrono::milliseconds(50));
        }
    );

    std::thread threadProducer(
        [&queImuData] {
            putData(queImuData, std::chrono::milliseconds(10));
            std::cout << "Producer stops making new data\n";
            queImuData.close();
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Queue is shutdown by main\n";
    queImuData.close();

    threadProducer.join();
    threadConsumer1.join();
    threadConsumer2.join();

    return EXIT_SUCCESS;
}
