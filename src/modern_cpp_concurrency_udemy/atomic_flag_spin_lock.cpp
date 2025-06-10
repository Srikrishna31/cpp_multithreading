//
// Created by coolk on 10-06-2025.
//
// Use std::atomic_flag to implement a spin lock which protects a critical section
#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

std::atomic_flag lock_cout = ATOMIC_FLAG_INIT;

auto task(int n) {
    // test_and_set()
    // Returns true if another thread set the flag.
    // Returns false if this thread set the flag.
    while (lock_cout.test_and_set()) {
        using namespace std::literals;

        std::this_thread::sleep_for(50ms);
        std::cout << "I'm a task with argument " << n << std::endl;

        // Clear the flag so that another thread can set it.
        lock_cout.clear();
    }
}

/**
 *
 * Pros and cons of spin lock
 *  * A spinning thread remains active
 *      # A mutex may block the thread
 *  * It can continue immediately when it "gets the lock"
 *      # With a mutex, the thread may need to be reloaded or woken up
 *  * Processor-intensive
 *      # Only suitable for protecting very short critical sections
 *      # And/or very low contention
 *      # Performance can be heavily impacted if spinning threads interrupt each other
 *      # Usually only used in operating systems and libraries.
 *
 *  Hybrid Mutex
 *    * Often used to implement std::mutex
 *    * Start with a spin lock with a time out.
 *      # If the thread sets the flag in time, enter the critical section
 *      # If the thread cannot set the flag in time, use the normal mutex implementation.
 *    * This gives better performance than the conventional implementation.
 */
auto main() -> int {
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread{task, i});
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}