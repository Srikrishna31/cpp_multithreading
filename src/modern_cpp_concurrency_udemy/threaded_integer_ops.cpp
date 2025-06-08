//
// Created by coolk on 08-06-2025.
//
// A shared variable is modified by multiple threads.
// Integer operations take single instruction.
// Is there a data race?
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>

// std::mutex mut;
// int counter = 0;

std::atomic<int> counter = 0;

// volatile int counter = 0;

auto task() {
    for (int i = 0; i < 100'000; ++i) {
        // std::lock_guard<std::mutex> lock(mut);
        ++counter;
    }
}
/**
 *  Integer operations and Threads
 *      - Integer operations are usually a single instruction
 *          > True on x64
 *          > Provided the data is correctly aligned and fits into a single word.
 *      - A thread cannot be interrupted while performing an integer operation
 *      - Is there a data race?
 *          > The output is a number like 511805 - a number less than 1000'000;
 *          > The operation ++ is a single instruction, but involves three operations
 *              # Pre-fetch the value of count
 *              # Increment the value in the processor's core register
 *              # Publish the new value of count.
 *          > The thread could use a stale value in its calculation
 *          > The thread could publish its result after a thread which ran later.
 *
 *  Atomic Keyword
 *      - The compiler will generate special instructions which
 *          > Disable pre-fetch for count
 *          > Flush the store buffer immediately after doing the increment
 *      - This also avoids some other problems
 *          > Hardware optimizations which change the instruction order.
 *          > Compiler optimizations which change the instruction order.
 *      - The result is that only one thread can access count at a time
 *      - This prevents the data race
 *          > It also makes the operations take much longer.
 *      - Atomic Types
 *          > All operations on the variable will be atomic
 *          > C++11 defines an atomic template
 *              # The parameter type must be a type which is "trivially copyable"
 *                  - Scalar type
 *                  - Class where all the copy and move constructors are trivial.
 *              # Normally only integer types and pointers are used.
 *              # For more complex types, locks may be silently added
 *                  - Locking a mutex takes much longer
 *                  - To avoid this, use a pointer to the type.
 *              # Using an std::atomic<T> object
 *                  - We can assign to and from the object.
 *                  - These are two distinct atomic operations: Other threads could interleave between them.
 *                  - Operations such as ++ are atomic
 *                      * Fetch old value
 *                      * Increment
 *                      * Store new value
 *      - Volatile keyword
 *          > May change a value without explicit modification
 *              # Prevents some compiler optimizations
 *              # Typically used when accessing hardware
 *          > Often mistakenly used in threading
 *              # Some programmers expect the Java/C# behavior
 *              # Has no impact on thread safety.
 */
auto main() -> int {
    std::vector<std::thread> tasks;

    for (int i = 0; i < 10; ++i) {
        tasks.emplace_back(task);
    }

    for (auto& t : tasks) {
        t.join();
    }

    std::cout << counter << std::endl;
}