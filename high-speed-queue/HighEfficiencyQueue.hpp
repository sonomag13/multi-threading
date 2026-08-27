#pragma once

#include <queue>

template<class T>
class HighEfficiencyQueue {
public:
    HighEfficiencyQueue() = default;
    ~HighEfficiencyQueue() = default;



private:
    std::queue<T> _queue;
};