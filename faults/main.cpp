// Common faults in multi-threading

#include <iostream>
#include <thread>

void func1(int & x) {

    x++;
}

void run_func1(int x) {

    std::thread t(func1, std::ref(x));
    // t.join(); without join, run_fun1 will keep running and return; local variable x will be
    // destroyed, meaning func1 running on thread t will write to an delocated space
    std::cout << "x = " << x << '\n';
}

int main() {

    // Example 1: use std::ref to mark that x is passed by reference
    int x1{5};
    std::thread t(func1, std::ref(x1));
    t.join();
    std::cout << "x1 = " << x1 << '\n';

    // Example 2:bffdsagew
    int x2{10};
    run_func1(x2);

    return 0;
}
