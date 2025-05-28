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
 */
auto main() -> int {
    std::thread thrA(funcA);
    std::this_thread::sleep_for(10ms);
    std::thread thrB(funcB);

    thrA.join();
    thrB.join();
}