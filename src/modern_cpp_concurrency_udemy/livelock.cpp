//
// Created by coolk on 28-05-2025.
//
// Livelock caused by poorly implemented deadlock avoidance. If the thread cannot get a lock, sleep and try again.
// However, all the threads wake up at the same time.
#include <thread>
#include <mutex>
#include <iostream>
#include <string>
#include <chrono>

using namespace std::literals;

std::mutex mut1, mut2;

auto funcA() {
    std::this_thread::sleep_for(10ms);
    auto locked = false;
    while (!locked) {
        std::lock_guard<std::mutex> lock(mut1);
        std::cout << "After you, Claude!" << std::endl;
        std::this_thread::sleep_for(2s);
        locked = mut2.try_lock();
    }
    if (locked)
        std::cout << "ThreadA has locked both mutexes" << std::endl;
}

auto funcB() {
    auto locked = false;
    while (!locked) {
        std::lock_guard<std::mutex> lock(mut2);
        std::cout << "After you, Cecil!" << std::endl;
        std::this_thread::sleep_for(2s);
        locked = mut1.try_lock();
    }
    if (locked) {
        std::cout << "ThreadB has locked both mutexes" << std::endl;
    }
}

auto funcA_livelock_resolved() {
    std::this_thread::sleep_for(10ms);

    std::cout << "After you, Claude!" << std::endl;
    std::scoped_lock scoped_lock(mut1, mut2);   // Lock both mutexes
    std::this_thread::sleep_for(2s);
    std::cout << "ThreadA has locked both mutexes" << std::endl;
}

auto funcB_livelock_resolved() {
    std::cout << "After you, Cecil!" << std::endl;
    std::scoped_lock scoped_lock(mut2, mut1);
    std::this_thread::sleep_for(2s);
    std::cout << "ThreadB has locked both mutexes" << std::endl;
}
/**
 *
 * LiveLock
 *  * A program cannot make progress
 *      > In deadlock, the threads are inactive
 *      > In livelock, the threads are still active
 *  * Livelock can result from badly done deadlock avoidance
 *      > A thread cannot get a lock
 *      > Instead of blocking indefinitely, it backs off and tries again.
 *  * Livelock Analogy
 *      > Imagine two very polite people
 *      > The walk down a corridor together
 *      > They reach a narrow door
 *          . They each try to go through the door at the same time.
 *          . Each one stops and waits for the other to go through the door.
 *          . Then they both try to go through the door at the same time.
 *          . Then each one stops and waits for the other to go through the door, etc.
 *
 *   * Livelock Avoidance
 *      > Use std::scoped_lock or std::lock
 *          . The thread can acquire multiple locks in a single operation
 *          . Built-in deadlock avoidance
 *      > Thread Priority
 *          . We could assign different priorities to threads
 *          . Not directly supported by C++
 *          . Most thread implementations allow it.
 *              # Accessible via std::thread's native_handle()
 *              # A high priority thread will run more often
 *              # A low priority thread will be suspended or interrupted more often.
 *          . The high priority thread will lock the mutex first
 *          . The low priority thread will lock the mutex afterwards.
 *      > Reasource Starvation
 *          . A thread cannot get the resources it needs to run
 *              # In deadlock and livelock, the thread cannot acquire a lock it needs
 *          . Lack of system resources can prevent a thread from starting
 *              # System memory exhausted
 *              # Maximum supported number of threads is already running
 *          . Low priority threads may get starved of processor time
 *              # Higher priority threads are given preference by the scheduler
 *              # Good schedulers try to avoid this.
 */
auto main() -> int {
    // Uncomment below for livelock example:
    // std::thread thrA(funcA);
    // std::this_thread::sleep_for(10ms);
    // std::thread thrB(funcB);
    //
    // thrA.join();
    // thrB.join();

    std::thread threadA(funcA_livelock_resolved);
    std::this_thread::sleep_for(10ms);
    std::thread threadB(funcB_livelock_resolved);

    threadA.join();
    threadB.join();

    return 0;
}