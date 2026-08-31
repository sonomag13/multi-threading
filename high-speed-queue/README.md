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

## Notes: Passing Values and References to `put`

### Recommended interface

Use two overloads when minimizing copies and moves is important:

```cpp
void put(const T& value) {
    {
        std::lock_guard lock(mutex);
        queue.emplace(value);             // copy into the queue
    }
    condition.notify_one();
}

void put(T&& value) {
    {
        std::lock_guard lock(mutex);
        queue.emplace(std::move(value));  // move into the queue
    }
    condition.notify_one();
}
```

Their intended meanings are:

```cpp
T value;
const T const_value;

queue.put(value);             // copy; value remains unchanged
queue.put(const_value);       // copy; const_value remains unchanged
queue.put(std::move(value));  // move; value remains valid but may be moved-from
queue.put(T{});               // move from a temporary
```

Prefer `const T&` over `T&` for the copy overload. A `T&` parameter permits the
function to modify the caller's object, cannot accept a const object, and cannot
accept a temporary. A `const T&` parameter provides read-only access and can bind
to mutable lvalues, const lvalues, and rvalues.

Do not implement `put(T&)` by applying `std::move` to its argument. A call such as
`put(value)` would then silently consume an ordinary lvalue even though the caller
did not explicitly write `std::move(value)`.

### Passing by value

An alternative is:

```cpp
void put(T value) {
    std::lock_guard lock(mutex);
    queue.emplace(std::move(value));
}
```

The parameter `value` is a new `T` object. For an lvalue argument this generally
means one copy into the parameter followed by one move into the queue. For an
rvalue it generally means a move into the parameter followed by a move into the
queue, although copy elision can remove parameter construction in some cases.

The two reference overloads can avoid that intermediate move:

| Call | `put(T value)` | Reference overloads |
|---|---|---|
| `put(value)` | one copy + one move | one copy |
| `put(std::move(value))` | up to two moves | one move |
| `put(T{})` | usually one move after elision | one move |

Passing by reference avoids constructing a parameter object, but it does not
avoid constructing the queue's own `T`. The queue must own a separate object, so
it must copy or move the argument into its storage.

### `const`, lvalues, and rvalues

`const` and value category describe different properties:

* `const` controls whether an object may be modified through an expression.
* Lvalue/rvalue category controls how an expression can bind to references and
  whether its resources may potentially be transferred.

`std::move` changes the expression's value category; it does not remove `const`:

```cpp
T value;
const T const_value;

std::move(value);        // T&&
std::move(const_value);  // const T&&
```

Given `put(const T&)` and `put(T&&)`, overload selection is:

| Argument | Selected overload |
|---|---|
| mutable lvalue `value` | `put(const T&)` |
| const lvalue `const_value` | `put(const T&)` |
| mutable rvalue `std::move(value)` or `T{}` | `put(T&&)` |
| const rvalue `std::move(const_value)` | `put(const T&)` |

A `const T&&` can bind to `const T&` because constness is preserved. It cannot
bind to `T&&`, because that would discard `const` and provide mutable access to a
const object. Ordinary move constructors take `T&&` because transferring resources
usually modifies the source. Consequently, attempting to move a const object
normally invokes its copy constructor instead:

```cpp
const T source;
T destination(std::move(source)); // normally calls T(const T&)
```

Finally, a named rvalue-reference parameter is itself an lvalue expression inside
the function. Explicit `std::move` is therefore required when inserting it:

```cpp
void put(T&& value) {
    queue.emplace(std::move(value));
}
```
