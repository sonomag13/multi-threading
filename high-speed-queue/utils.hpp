//
// Created by Lu Gao on 9/1/26.
//

#pragma once

#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

#include "IMUData.hpp"

template<class QueueType>
void putData(QueueType& queue, std::chrono::milliseconds sleepTime_ms) {

    for (std::size_t i = 0; i < 100; ++i) {
        IMUData imuData;
        if (!queue.put(imuData)) {
            break;
        }
        std::this_thread::sleep_for(sleepTime_ms);
    }
}

template<class QueueType>
void getData(QueueType& queue, std::chrono::milliseconds sleepTime_ms) {

    while (auto val = queue.get()) {
        std::cout << "time stamp = " << val->timestamp << '\n';
        std::this_thread::sleep_for(sleepTime_ms);
    }
}
