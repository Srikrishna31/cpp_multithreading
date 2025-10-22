#include <chrono>
#include <condition_variable>
#include <iostream>
#include <vector>

class Semaphore {
    private:
    std::condition_variable cv;
    std::mutex mtx;
    unsigned int counter{0};
    unsigned int max_count{1};

public:
    Semaphore(unsigned int count = 0) : counter(count) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Removing one item" << std::endl;
        while (counter == 0) {
            cv.wait(lock);
        }
        // cv.wait(lock, [this] { return counter > 0; });
        --counter;
        count();
    }

    void signal() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Adding one item" << std::endl;
        if (counter < max_count){
            ++counter;
            count();
        }
        cv.notify_all();
    }

    void acquire() {
        wait();
    }

    void release() {
        signal();
    }

    void count() const {
        std::cout << "Value of counter: " << counter << std::endl;
    }
};

/**
 *
 * Binary Semaphore as Mutex
 *  - The counter can only have two values: 0 and 1.
 *  - Used for mutual exclusion.
 *  - To lock it call acquire()/wait()
 *      * The counter is decremented to 0
 *      * Other threads that call acquire()/wait() will be blocked.
 *  - To unlock it call release()/signal()
 *      * The counter is incremented to 1
 *      * One of the other threads that called acquire()/wait() can now continue.
 *
 * Binary Semaphore as Condition Variable
 *  - Also used for signalling
 *      * Can be used as a replacement for condition variables
 *  - To wait for a signal, a thread calls acquire()/wait()
 *      * The counter is decremented to 0
 *      * The thread waits for another thread to increment it.
 *  - To notify a waiting thread, call release()/signal()
 *      * The counter is incremented to 1
 *      * The waiting thread can now continue.
 *  - To notify multiple threads, use a suitable value for max_count.
 *
 * Semaphores
 *  - More flexible
 *      * Can notify any given number of waiting threads.
 *  - Simpler code
 *      * Avoids working with mutexes and condition variables.
 *  - Performance
 *      * Can be implemented using atomic operations, can often be faster.
 *  - More versatile
 *      * Can be used to create more complex synchronization objects( synchronization, counting, and signaling).
 *      * "Little book of semaphores"
 */
auto main() -> int {
    Semaphore sem;

    auto  insert = [&sem]() {
        sem.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };

    auto relinquish = [&sem]() {
        sem.acquire();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };

    std::vector<std::thread> tasks;

    for (int i = 0; i < 2; ++i) {
        tasks.push_back(std::thread(insert));
    }
    for (int i = 0; i < 4; ++i) {
        tasks.push_back(std::thread(relinquish));
    }
    for (int i = 0; i < 3; ++i) {
        tasks.push_back(std::thread(insert));
    }

    for (auto& task : tasks) {
        task.join();
    }

    return 0;
}
