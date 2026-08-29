//
// Created by Lu Gao on 8/28/26.
//

#pragma once

#include <chrono>
#include <cstdint>

struct IMUData {

public:
    IMUData():
        timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()} {

    }

    ~IMUData() = default;

    std::chrono::milliseconds timestamp;
};