//
// Created by Lu Gao on 9/21/24.
//

#pragma once

#include <chrono>
#include <iostream>
#include <thread>

// Function to be used as a task
template<class T>
void printNumber(T val) {
    /**
     * Print a number
     */

    std::cout << "thread " << std::this_thread::get_id()  << ": value = " << val << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

template<class T>
T addNumbers(const T x1, const T x2) {
    /**
     * Add two number and return the sum
     */

    T sum{x1 + x2};
    std::cout << "thread " << std::this_thread::get_id() << ": x1 + x2 = " << sum << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return sum;
}

