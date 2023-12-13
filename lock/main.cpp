#include <iostream>
#include "LockMutex.h"

int main() {

    std::cout << "Conventional mutex" << '\n';

    LockMutex exampleLockMutex;
    exampleLockMutex.addNumWithoutMutex();
    exampleLockMutex.reset();
    exampleLockMutex.addNumWithMutex();
    exampleLockMutex.reset();
    exampleLockMutex.addNumWithAtomic();
    exampleLockMutex.reset();
    exampleLockMutex.addNumWithLockGuard();

    return 0;
}
