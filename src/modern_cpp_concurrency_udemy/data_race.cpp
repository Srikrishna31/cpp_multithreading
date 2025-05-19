//
// Created by coolk on 19-05-2025.
//

#include <iostream>
#include <ostream>
#include <thread>
static int count = 0;

auto increment() -> void {
    for (int i = 0; i < 100000; i++) {
        count++;
    }
}

/**
 *
 *   Data Race
 *
 *   -> A "data race" occurs when:
 *      * Two or more threads access the same memory location
 *      * And at least one of the threads modifies it.
 *      * Potentially conflicting accesses to the same memory location.
 *
 *   -> Only safe if the threads are synchronized
 *      * One thread accesses the memory location at a time.
 *      * The other threads have to wait until it is safe for them to access it.
 *      * In effect, the threads execute sequentially while they access it.
 *
 *   -> A data race causes undefined behavior
 *      * The program is not guaranteed to behave consistently.
 *
 *   Race Condition
 *
 *   -> The outcome is affected by timing changes
 *   -> A data race is a special case of a race condition.
 *      * The outcome depends on when threads are scheduled to run.
 *
 *   Torn Writes and Reads
 *
 *      -> A "torn" write
 *          * Can occur when writing data requires multiple operations
 *          * Another writing thread interleaves between the write operations.
 *      -> A "torn" read
 *          * Can occur when reading and writing data requires multiple operations.
 *          * A writing thread interleaves between the read operations
 *      -> Improper Construction
 *          * Thread A creates a shared object, by calling the class's constructor.
 *          * Thread B interleaves and it also calls the class's constructor, at the same memory location.
 *          * We have a torn write: The object will be initialized with a mixture of values from each thread.
 *
 *      -> Data Races can be very difficult to detect and replicate
 *          * Intermittent errors
 *          * Sensitive to environment
 *          * Often dependent on timing coincidences or system load.
 */
auto main() -> int {
    std::thread t1(increment);
    std::thread t2(increment);
    std::thread t3(increment);
    std::thread t4(increment);
    std::thread t5(increment);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    std::cout << count << std::endl;

    return 0;
}