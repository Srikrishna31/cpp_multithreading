//
// Created by coolk on 25-05-2025.
//
#include <iostream>
#include <mutex>

class Test {
    public:

    Test() {
        std::cout << "Test()" << std::endl;
    }

    void operator()() const {
        std::cout << "Function call operator called!" << std::endl;
    }
};

Test *ptest = nullptr;      // Variable to be lazy initialized.
std::mutex m;

Test* ptest2 = nullptr;
std::once_flag flag;    // The flag stores synchronization data.

/**
 * Thread-safe lazy initialization
 *      * Every thread that calls process() locks the mutex
 *          . Locking the mutex blocks every other thread that calls process()
 *      * The lock is only needed while ptest is being initialized
 *          . Once ptest has been initialized, locking the mutex is unnecessary
 *          . Causes a loss of performance.
 *
 *
 *      * Double-checked locking algorithm
 *          -> More efficient version of thread-safe lazy initialization
 */
void process() {
    std::unique_lock<std::mutex> lock(m); //Protect ptest

    if (!ptest) {
        ptest = new Test;
    }
    lock.unlock();
    ptest->operator()();
}

/**
 * With this simple implementation of double-checked locking, there is still a possibility of a race-condition
 *      * The initialization of ptest involves several operations
 *          -> Allocate enough memory to store a Test object.
 *          -> Construct a Test object in the memory
 *          -> Store the address in ptest
 *      * C++ allows these to be performed in a different order, e.g.
 *          -> Allocate enough memory to store a Test object.
 *          -> Store the address in ptest.
 *          -> Construct a Test object in the memory
 *
 *      * Undefined behaviour
 *          -> Thread A checks ptest and locks the mutex
 *          -> Thread A allocates the memory and assigns to ptest
 *          -> However, it has not yet called the constructor
 *          -> Thread B checks ptest and it is not null
 *          -> Thread B doesnot lock the mutex
 *          -> Thread B jumps out of the if statement
 *          -> Thread B calls a member function of an uninitialized object: Undefined behavior
 *
 *      * std::call_once
 *          -> One way to solve the above undefined behavior is to use std::call_once()
 *              . A given function is only called once
 *              . It is done in one thread
 *              . The thread cannot be interrupted until the function call completes.
 *          -> We use it with a global instance of std::once_flag
 *          -> We pass the instance and the function to std::call_once()
 */
auto double_checked_process() {
    if (!ptest) {
        std::lock_guard<std::mutex> lock(m);
        if (!ptest) {
            ptest = new Test;
        }
    }

    ptest->operator()();
}

/**
 * Double-checked locking with std::call_once
 *      . Threadsafe
 *      . Less overhead than a mutex.
 *
 * Double-checked locking in C++17
 *     * C++17 defines the order of initialization
 *          . Allocate enough memory to store a Test object.
 *          . Construct a Test object in the memory
 *          . Store the address in ptest
 *     * Double-checked locking no longer causes a data race.
 */
auto process_with_call_once() {
    std::call_once(flag, []() {
        ptest = new Test;
    });
    ptest->operator()();
}

/**
 *
 * Meyer's singleton pattern (function local static)
 *
 * Conclusion:
 *  * Four ways to do thread-safe lazy initialization
 *      . Naive use of a mutex
 *      . Use std::call_once()
 *      . Double-checked locking with a C++17 compiler or later
 *      . Meyers singleton with static local variable.
 *
 *  * Recommendations
 *      . Use Meyers singleton, if ptest is not required outside the function.
 *      . Otherwise use std::call_once()
 */
void process_singleton() {
    static Test ptest;      // Variable which is lazily initialized

    ptest();
}

auto main() -> int {
    // process();

    std::thread t(process_with_call_once);
    std::thread t2(process_with_call_once);
    t.join();
    t2.join();

    process_singleton();
    return 0;
}
