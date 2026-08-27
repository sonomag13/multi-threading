#include <iostream>

#include "HighEfficiencyQueue.hpp"

int main() {

    int x = 1;
    int y = 1;
    auto x2 = ++x;
    auto y2 = y++;

    std::cout << "x = " << x << std::endl;
    std::cout << "x2 = " << x2 << std::endl;
    std::cout << "y = " << y << std::endl;
    std::cout << "y2 = " << y2 << std::endl;

    return EXIT_SUCCESS;
}
