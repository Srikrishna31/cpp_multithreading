//
// Created by coolk on 25-05-2025.
//

#include <iostream>
#include <ostream>
#include <random>
#include <thread>
/**
*  * Thread-local Variables
*      . Same as static and global variables, or data members of a class or local variables in a function
*      . However, there is a separate object for each thread.
*      . We use the thread_local keyword to declare them
*
*  * Thread-local Variable Lifetimes
*      . Global and namespace scope
*          # Always constructed at or before the first use in a translation unit.
*          # It is safe to use them in dynamic libraries (DLL/.so/dylib)
*      . Local variables
*          # Initialized in the same way as static local variables.
*      . In all cases they are destroyed when the thread completes its execution.
*/
thread_local std::mt19937 mt;

void func() {
    /**
     * We can make a random number engine thread-local, which ensures that each thread generates the same sequence
     *      -> Useful for testing.
     */
    std::uniform_real_distribution<double> dist{0,1};

    for (int i = 0; i < 10; ++i) {
        std::cout << dist(mt) << std::endl;
    }
}

auto main() -> int {
    std::cout << "Thread 1's random values: " << std::endl;
    std::thread t1{func};
    t1.join();

    std::cout << "Thread 2's random values: " << std::endl;
    std::thread t2{func};
    t2.join();

    return 0;
}