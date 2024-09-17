//// practice of using condition variable


#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

std::queue<int> queTasks;
std::condition_variable cv;
std::mutex mtx;
bool completed{false};

void Producer(size_t idx, size_t numTasks) {
    /**
     * thread of producer to add tasks to the queue
     */

    for (size_t i = 0; i < numTasks; ++i) {
        {
            /**
             * use this local scope so that we do not need to unlock
             **/

            std::unique_lock<std::mutex> lock(mtx);
            queTasks.push(static_cast<int>(i));
            std::stringstream ss;

            // sleep to simulate the time to produce a task
            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // this simulates the time to produce a task
            ss << "[Producer " << idx << "] has enqueued task " << i << '\n';
            std::cout << ss.str();

            cv.notify_all();
        }

        // give a short time window for the worker to lock the mutex
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        completed = true;
    }

}

void Worker(size_t idx) {
    /**
     * thread of worker to grab tasks from the queue
     */

    while(true) {

        std::unique_lock<std ::mutex> lock(mtx);
        cv.wait(lock, [] () {
            return !queTasks.empty() || completed;
        });

        if (completed && queTasks.empty()) {
            std::stringstream ss;
            ss << "[Thread " << idx << "] finds tasks completed\n";
            std::cout << ss.str();
            return;
        }

        if (!queTasks.empty()) {
            // not completed and the queue is not empty, then let us get a task
            int taskId{queTasks.front()};
            queTasks.pop();

            lock.unlock();

            // sleep to simulate time to do the work
            std::stringstream ss;
            ss << "[Thread " << idx << "] works on task " << taskId << '\n';
            std::cout << ss.str();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        cv.notify_all();
    }

}

int main(int argc, char** argv) {

    const size_t numTasks{static_cast<size_t>(std::stoi(argv[1]))};

    std::thread producer1(Producer, 1, numTasks);
    std::thread producer2(Producer, 2, numTasks);
    std::thread worker1(Worker, 1);
    std::thread worker2(Worker, 2);

    producer1.join();
    producer2.join();
    worker1.join();
    worker2.join();

    return 0;
}
