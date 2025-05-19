//
// Created by coolk on 17-05-2025.
//

#include <iostream>
#include <string>
#include <thread>

auto fizzbuzz(const uint32_t val) {
    const auto div_by_3 = val % 3 == 0;
    const auto div_by_5 = val % 5 == 0;

    if (div_by_3 && div_by_5) {
        std::cout << "fizzbuzz" << std::endl;
        // return "fizzbuzz";
    }
    else if (div_by_5) {
        std::cout << "buzz" << std::endl;
        // return "buzz";
    }
    else if (div_by_3) {
        std::cout << "fizz" << std::endl;
        // return "fizz";
    } else {
        std::cout << val << std::endl;
    }

    // return std::to_string(val);
}

/**
 *
 * Thread Function with Arguments
 *
 * The std::thread object owns the arguments
 *      * lvalue arguments are passed by value
 *      * rvalue arguments are passed by move
 *
 * An std::thread object has ownership of an execution thread
 *  * Only one object can be bound to an execution thread at a time.
 *
 * Threads and Exceptions
 *
 *  -> Each thread has its own execution stack
 *  -> The stack is unwound when the thread throws an exception
 *      * The destructors for all objects in scope are called
 *      * The program moves up the thread's stack until it finds a suitable handler
 *      * If no handler is found, the program is terminated
 *  -> Other threads in the program cannot catch the exception
 *      * Including the parent thread and the main thread
 *  -> Exceptions can only be handled in the threads where they occur
 *      * Use a try/catch block in the normal way.
 *
 * Detaching a Thread
 *  -> Instead of calling join(), we can call detach()
 *      * The parent thread will continue executing
 *      * The child thread will run until it completes
 *      * Or the program terminates
 *      * Analogous to "daemon" process
 *  -> When an execution thread is detached
 *      * The std::thread object is no longer associated with it.
 *      * The destructor will not call std::terminate()
 *
 *  Exception in Parent Thread
 *   -> The destructors are called for every object in scope
 *      * Including std::thread's destructor
 *      * This checks whether join() or detach() have been called.
 *      * If neither, it calls std::terminate()
 *   -> We must call either join() or detach() before the thread is destroyed in all paths through the code.
 *
 */
int main () {
    std::thread t1(fizzbuzz, 10);
    std::thread t2(fizzbuzz, 15);

    t1.join();
    t2.join();

    return 0;
    // std::cout << fizzbuzz(1) << std::endl;
    // std::cout << fizzbuzz(2) << std::endl;
    // std::cout << fizzbuzz(3) << std::endl;
    // std::cout << fizzbuzz(4) << std::endl;
    // std::cout << fizzbuzz(5) << std::endl;
    // std::cout << fizzbuzz(6) << std::endl;
    // std::cout << fizzbuzz(7) << std::endl;
    // std::cout << fizzbuzz(8) << std::endl;
    // std::cout << fizzbuzz(15) << std::endl;
}