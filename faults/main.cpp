// Common faults in multi-threading

#include <iostream>
#include <thread>

void func1(int & x) {

    x++;
}

int main() {
    int x {5};
    std::thread t(func1, std::ref(x));

    std::cout << "x = " << x << '\n';
    return 0;
}
