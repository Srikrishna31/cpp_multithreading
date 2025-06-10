//
// Created by coolk on 08-06-2025.
//
#include <thread>
#include <atomic>
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

std::atomic<Test*> ptest = nullptr;
std::mutex mut;

auto double_checked_process() {
    if (!ptest) {
        std::lock_guard<std::mutex> lock(mut);
        if (!ptest) {
            ptest = new Test();
        }
    }
    const Test* ptest2 = ptest;
    ptest2->operator()();
}
/**
 * Double-checked locking
 *      > One solution is to make the initialized object atomic
 *      > Atomic types donot support the . or -> operators
 *      > We must copy to a non-atomic pointer before we can use it.
 *          Test* ptr = ptest;  // atomic<Test*> ptest = nullptr;
 *          ptr->func();
 *
 * Member functions for atomic types
 *      > store(): Atomically replace the object's value with its argument
 *      > load(): Atomically return the object's value
 *      > operator=()/operator T() : Synonyms for store and load.
 *      > exchange()
 *          # Atomically replace the object's value with it's argument
 *          # Return the previous value;
 *      > Atomic pointers support pointer arithmetic
 *          # increment and decrement operators
 *          # fetch_add() synonym for x++
 *          # fetch_sub() synonym for x--
 *          # += and -= operators
 *      > Integer specializations have these plus, atomic bitwise operations: &, | and ^
 *
 *
 *
 */
auto main() -> int {
    std::thread t1(double_checked_process);
    std::thread t2(double_checked_process);

    t1.join();
    t2.join();

    std::atomic<int> x = 0;
    std::cout << "After initialization: x = " << x << std::endl;

    x = 2;
    // Atomic assignment from x. y can be non-atomic.
    int y = x;

    std::cout << "After assignment: x = " << x << ", y  = " << y << std::endl;

    x.store(4);
    std::cout << "After store: x = " << x.load() << std::endl;

    std::cout << "Exchange returns: " << x.exchange(y) << std::endl;
    std::cout << "After exchange: x = " << x << ", y = " << y << std::endl;
    return 0;
}