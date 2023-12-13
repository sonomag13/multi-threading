#include <chrono>
#include <iostream>
#include <thread>

void printHelloWorld() {

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Hello World~!" << '\n';
}

int main() {

    std::thread thPrint(printHelloWorld);
    thPrint.join();
    return EXIT_SUCCESS;
}