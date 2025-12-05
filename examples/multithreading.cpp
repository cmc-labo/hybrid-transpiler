/**
 * Multithreading Examples
 * Demonstrates various C++ threading patterns and their conversion to Rust/Go
 */

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>

/**
 * Example 1: Basic thread creation and joining
 */
class SimpleThreadExample {
private:
    int data;

public:
    SimpleThreadExample() : data(0) {}

    void workerFunction(int value) {
        data += value;
    }

    void runThreads() {
        std::thread t1(&SimpleThreadExample::workerFunction, this, 10);
        std::thread t2(&SimpleThreadExample::workerFunction, this, 20);

        // Wait for threads to complete
        t1.join();
        t2.join();
    }
};

/**
 * Example 2: Thread-safe counter with mutex
 */
class ThreadSafeCounter {
private:
    int count;
    std::mutex mutex;

public:
    ThreadSafeCounter() : count(0) {}

    void increment() {
        std::lock_guard<std::mutex> lock(mutex);
        ++count;
    }

    void decrement() {
        std::lock_guard<std::mutex> lock(mutex);
        --count;
    }

    int getValue() {
        std::lock_guard<std::mutex> lock(mutex);
        return count;
    }
};

/**
 * Example 3: Atomic operations
 */
class AtomicCounter {
private:
    std::atomic<int> count;

public:
    AtomicCounter() : count(0) {}

    void increment() {
        count.fetch_add(1);
    }

    void decrement() {
        count.fetch_sub(1);
    }

    int getValue() {
        return count.load();
    }
};

/**
 * Example 4: Producer-Consumer with condition variable
 */
class ProducerConsumer {
private:
    std::vector<int> buffer;
    std::mutex mutex;
    std::condition_variable cv;
    const size_t MAX_BUFFER_SIZE = 10;
    bool done = false;

public:
    void produce(int value) {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait until buffer has space
        cv.wait(lock, [this] { return buffer.size() < MAX_BUFFER_SIZE; });

        buffer.push_back(value);
        cv.notify_one();
    }

    int consume() {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait until buffer has data
        cv.wait(lock, [this] { return !buffer.empty() || done; });

        if (buffer.empty()) {
            return -1; // No more data
        }

        int value = buffer.back();
        buffer.pop_back();
        cv.notify_one();

        return value;
    }

    void finish() {
        std::unique_lock<std::mutex> lock(mutex);
        done = true;
        cv.notify_all();
    }
};

/**
 * Example 5: Shared mutex for reader-writer lock
 */
class SharedData {
private:
    int data;
    mutable std::shared_mutex mutex;

public:
    SharedData() : data(0) {}

    // Multiple readers can read simultaneously
    int read() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return data;
    }

    // Only one writer at a time
    void write(int value) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        data = value;
    }
};

/**
 * Example 6: Detached thread
 */
class DetachedThreadExample {
public:
    void backgroundTask() {
        // Do some work
    }

    void launchBackgroundTask() {
        std::thread t(&DetachedThreadExample::backgroundTask, this);
        t.detach(); // Thread runs independently
    }
};

/**
 * Example 7: Multiple threads with lambda
 */
void parallelComputation() {
    std::atomic<int> result(0);
    const int NUM_THREADS = 4;

    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&result, i]() {
            // Each thread adds its index to result
            result.fetch_add(i);
        });
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
}

/**
 * Example 8: Thread pool pattern
 */
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop;

public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    cv.wait(lock, [this] { return stop; });

                    if (stop) {
                        return;
                    }

                    // Process task
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }

        cv.notify_all();

        for (auto& worker : workers) {
            worker.join();
        }
    }
};
