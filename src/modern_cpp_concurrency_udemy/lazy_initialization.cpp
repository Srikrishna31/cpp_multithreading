//
// Created by coolk on 25-05-2025.
//
#include <iostream>
#include <mutex>

class Test {
    public:
    void operator()() const {
        std::cout << "Function call operator called!" << std::endl;
    }
};

Test *ptest = nullptr;      // Variable to be lazy initialized.
std::mutex m;

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
    ptest();
}

auto main() -> int {
    process();
}