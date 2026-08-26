## Tesla-Style C++ Interview Problem: High-Performance Thread-Safe Frame Queue

### Background

A camera continuously captures image frames. The camera capture thread produces frames at a high rate, while a separate processing thread consumes the frames.

Because image frames can be large and the camera operates continuously, the queue must be both thread-safe and efficient under concurrent access.

### Task

Implement a generic thread-safe queue in C++ that can be used to pass image frames from a producer thread to a consumer thread.

```cpp
template <typename T>
class ThreadSafeQueue {
public:
    void push(T value);
    std::shared_ptr<T> pop();
};
```

Example usage:

```cpp
ThreadSafeQueue<Frame> frames;

// Camera thread
frames.push(captureFrame());

// Processing thread
auto frame = frames.pop();
if (frame) {
    processFrame(*frame);
}
```

### Requirements

1. **FIFO Ordering**

   Frames must be returned in the same order in which they were inserted.

2. **Thread Safety**

   `push()` and `pop()` may be called concurrently from different threads.

3. **High Concurrency**

   Avoid protecting the entire queue with a single mutex.

   The producer should be able to add a frame to the **tail** while the consumer simultaneously removes a frame from the **head** whenever possible.

4. **Two-Mutex Design**

   Use separate synchronization for the two ends of the queue:

```cpp
std::mutex head_mutex;
std::mutex tail_mutex;
```

The design should minimize contention between producers and consumers.

5. **Efficient Object Handling**

   Image frames may contain large amounts of data. Avoid unnecessary copies.

   The queue should correctly support movable types and may use C++ facilities such as:

```cpp
std::move
std::forward
std::is_constructible
std::is_move_constructible
```

Include `<type_traits>` where appropriate.

6. **Empty Queue**

   `pop()` must safely handle an empty queue without introducing a race condition between the head and tail.

7. **Memory Safety**

   Queue nodes must be correctly destroyed after removal. Use RAII and smart pointers where appropriate.

### Constraints

Do not simply implement the queue as:

```cpp
std::mutex mutex;
std::queue<T> queue;
```

with one mutex protecting every operation.

The goal is to design the underlying data structure so that operations on the head and tail can proceed independently.

### Discussion Questions

* Why is a single mutex correct but potentially inefficient?
* Why can separate head and tail mutexes improve throughput?
* How do you determine whether the queue is empty without creating a data race?
* What happens when `push()` and `pop()` execute at exactly the same time?
* Why might a dummy node simplify the implementation?
* Why are move semantics important when `T` represents an image frame?
* What purpose could `<type_traits>` serve in a generic queue implementation?
* How would you modify the implementation so that `pop()` blocks until a frame becomes available?
* How would the design change for multiple producers and multiple consumers?
