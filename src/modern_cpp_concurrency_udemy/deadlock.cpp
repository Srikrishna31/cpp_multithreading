//
// Created by coolk on 20-05-2025.
//

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
/**
 *
 * Mutex
 *  * MUTual EXclusion object
 *  * We can use a mutex object to implement locking.
 *  * A mutex has two states
 *      . "locked"
 *      . "unlocked"
 *  * Exclusion
 *      . The mutex is used to exclude threads from the critical section
 *  * Mutual
 *      . The threads agree to respect the mutex
 *  * Locking
 *      . If the mutex is unlocked, a thread can enter the critical section
 *      . If the mutex is locked, no thread can enter until it becomes unlocked.
 *      . A thread locks the mutex when it enters the critical section
 *      . A thread unlocks the mutex when it leaves the critical section.
 *  * Thread Synchronization with Mutex
 *      . The threads are synchronized
 *          # They cannot interleave when they execute in the critical section
 *          # There is no data race
 *      . Unlocking a mutex "publishes" any changes
 *          # Thread A modifies shared data
 *          # The new value is now available to other threads
 *          # It will be seen by the next thread which accesses the shared data.
 *  * Acquire-Release Semantics
 *      . A thread locks a mutex
 *          # It "acquires" exclusive access to the critical section
 *      . The thread unlocks the mutex
 *          # It "releases" exclusive access to the critical section
 *          # It also "releases" the result of any modifications
 *          # The next thread that locks the mutex will acquire these results.
 *      . These acquire-release semantics impose ordering on the threads.
 *          # There is no data race
 *          # The shared data is always in a consistent state.
 *
 *  * When Exception is Thrown in Critical Section
 *      . The destructors are called for all objects in scope
 *      . The program flow jumps into the catch handler
 *      . The unlock call is never executed
 *      . The mutex remains locked.
 *      . All other threads which are waiting to lock the mutex are blocked.
 *      . If main() is joined on these blocked threads, main() and in effect entire program is blocked.
 *
 *  * Read-write Lock
 *      . It would be useful to have "selective" locking
 *          # Lock when there is a thread which is writing
 *          # Do not lock when there are only reading threads
 *          # Often called a "read-write lock"
 *
 *  * std::shared_mutex: It can be locked in two different ways
 *      . Exclusive lock
 *          # No other thread may acquire a lock
 *          # No other thread can enter a critical section
 *      . Shared lock
 *          # Other threads may acquire a shared lock
 *          # They can execute critical sections concurrently
 *          # IT can only acquire an exclusive lock when the mutex is  unlocked
 *              -> If other threads have shared or exclusive locks, it must wait until all locks are released.
 *  * Pros and Cons of std::shared_mutex
 *      . Uses more memory than std::mutex
 *      . Slower than std::mutex
 *      . Best suited to situations where
 *          # Reader threads greatly outnumber writer threads
 *          # Read operations take a long time.
 *
 *  * Shared Data
 *      . Global Variable: Accessible to all code in the program
 *      . Static variable at namespace scope: Accessible to all code in the translation unit
 *      . Class member which is declared static
 *          # Potentially accessible to code which calls its member functions
 *          # If public, accessible to all code.
 *      . Local variable declared static: Accessible to all code which calls that function.
 *
 *  * Shared Data initialization
 *      . Global variable, static variable at namespace scope, static member of class
 *          # All are initialized when the program starts
 *          # At that point, only one thread is running.
 *          # There cannot be data race.
 *      . Static local variable
 *          # Initialized after the program starts
 *          # When the declaration is reached
 *          # Two or more threads may call the constructor concurrently, leading to possible datarace.
 *          # Static local variable initialization in C++11 and beyond
 *              -> The behavior is now well-defined
 *              -> Only one thread can initialize the variable
 *                  * Any other thread that reaches the declaration is blocked.
 *                  * Must wait until the first thread has finished initializing the variable.
 *                  * The threads are synchronized by the implementation
 *                  * No data race
 *              -> Subsequent modification
 *                  # The usual rules for shared data
 */
class Singleton {
    static Singleton* instance;

    Singleton() {std::cout << std::endl << "Initializing singleton" << std::endl;}
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static Singleton* get_Singleton() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }

    static Singleton& get_Instance() {
        // Initialized by the first thread that executes this code.
        static Singleton instance;

        return instance;
    }
};

Singleton* Singleton::instance = nullptr;

void task() {
    const Singleton* single = Singleton::get_Singleton();
    std::cout << single << std::endl;
}

/**
 *
 * Deadlock
 *      * A thread is deadlocked when it cannot run
 *      * Often used to refer to "mutual deadlock"
 *          - Two or more threads are waiting for each other
 *          - Threads A and B are waiting for an event that can never happen
 *      * The classic example involves waiting for mutexes
 */

using namespace std::literals;

std::mutex mut1;
std::mutex mut2;

auto funcA() {
    std::cout << "Thread A trying to lock mutex 1..." << std::endl;
    std::lock_guard<std::mutex> lock(mut1);
    std::cout << "Thread A has locked mutex 1" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A trying to lock mutex 2..." << std::endl;
    std::lock_guard<std::mutex> lock1(mut2);
    std::cout << "Thread A has locked mutex 2" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releases all its locks" << std::endl;
}

auto funcB() {
    std::cout << "Thread B trying to lock mutex 2..." << std::endl;
    std::lock_guard<std::mutex> lock(mut2);
    std::cout << "Thread B has locked mutex 2" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B trying to lock mutex 1..." << std::endl;
    std::lock_guard<std::mutex> lock1(mut1);
    std::cout << "Thread B has locked mutex 1" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releases all its locks" << std::endl;
}

auto main() -> int {
    std::vector<std::thread> threads;

    for (int i = 0; i < 30; i++) {
        // threads.emplace_back(task);
        threads.push_back(std::thread(task));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::thread thrA{funcA};
    std::thread thrB{funcB};

    thrA.join();
    thrB.join();
}
