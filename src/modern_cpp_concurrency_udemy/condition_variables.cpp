//
// Created by coolk on 29-05-2025.
//
// Condition variable example
//
// The reader thread waits for a notification
// The writer thread modifies the shared variable "data"
// The writer thread sends a notification
// The reader thread receives the notification and resumes
#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>
#include <chrono>

using namespace std::literals;

std::string data;

std::mutex mut;

std::condition_variable cond_var;

bool condition = false;

auto reader() {
    // Lock the mutex
    std::cout << "Reader thread locking mutex" << std::endl;
    std::unique_lock<std::mutex> lock(mut);
    std::cout << "Reader thread has locked the mutex" << std::endl;

    //Call wait()
    // This will unlock the mutex and make this thread sleep until the condition variable wakes us up
    std::cout << "Reader thread sleeping..." << std::endl;
    cond_var.wait(lock, [] {return condition;});

    // The condition variable has woken this thread up and locked the mutex
    std::cout << "Reader thread wakes up" << std::endl;

    // Display the new value of the string
    std::cout << "Data is \"" << data << "\"" << std::endl;
}

// Notifying thread
auto writer() {
    {
        std::cout << "Writer thread locking mutex" << std::endl;

        // Lock the mutex.
        // This will not be explicitly unlocked, std::lock_guard is sufficient
        std::lock_guard<std::mutex> lock(mut);
        std::cout << "Writer thread has locked the mutex" << std::endl;

        // Pretend to be busy
        std::this_thread::sleep_for(2s);

        // Modify the string
        std::cout << "Writer thread modifying data..." << std::endl;
        data = "Populated";

        condition = true;
        std::cout << "Writer thread unlocks the mutex" << std::endl;
    }

    // Notify the condition variable
    std::cout << "Writer thread sends notification" << std::endl;
    cond_var.notify_all();

    /*
     * for (int i = 0; i < 2; ++i) {
     *  cond_var.notify_one();
     * }
     */
}
/**
 *
 * std::condition_variable
 *      . wait()
 *          # Takes an argument of type unique_lock
 *          # It unlocks the argument and blocks the thread until a notification is received.
 *      . wait_for() and wait_until()
 *          # Re-lock their argument if a notification is not received in time
 *      . notify_one()
 *          # Wake up one of the waiting threads
 *          # The scheduler decides which thread is woken up.
 *      . notify_all()
 *          # Wake up all the waiting threads.
 *
 *      . Lost Wakeup
 *          # wait() will block  until the condition variable is notified.
 *      . If the writer calls notify() before the reader calls wait()
 *          # The condition variable is notified when there are no threads waiting
 *          # The reader will never be woken up
 *          # The reader could be blocked forever
 *      . This is known as a lost wakeup.
 *
 *      . Spurious Wakeup
 *          # Occasionally, the reader will be "spuriously" woken up
 *              * The reader has called wait()
 *              * The writing thread has not called notify()
 *              * The condition variable wakes the reader up anyway
 *          # This is due to the way that std::condition_variable is implemented
 *              * Avoiding spurious wakeups adds too much overhead
 *
 *      . wait() with Predicate
 *          # wait() takes an optional second argument - A predicate
 *          # Typically, the predicate checks a shared bool
 *              . The bool is initialized to false
 *              . It is set to true when the writer sends the notification
 *          # The reader thread will call this predicate
 *          # It will only call wait() if the predicate returns false
 */
auto main() -> int {
    data = "Empty";

    std::cout << "Data is \"" << data << "\"" << std::endl;

    std::thread read1{reader};
    std::thread write{writer};
    std::this_thread::sleep_for(10ms);
    std::thread read2{reader};
    std::this_thread::sleep_for(10ms);
    std::thread read3{reader};
    std::this_thread::sleep_for(10ms);

    write.join();
    read1.join();
    read2.join();
    read3.join();

    return 0;
}