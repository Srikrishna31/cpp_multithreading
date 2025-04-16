//
// Created by coolk on 13-04-2025.
//

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 *
 * The most basic mechanism for waiting for an event to be triggered by another thread is the condition variable. Conceptually
 * a condition variable is associated with some event or other condition, and one or more threads can wait for that condition
 * to be satisfied. When some thread has determined that the condition is satisfied, it can then notify one or more of the
 * threads waiting on the condition variable, in order to wake them up and allow them to continue processing.
 *
 * The Standard C++ library provides two implementations of a condition variable: std::condition_variable and
 * std::condition_variable_any. In both cases, they need to work with a mutex in order to provide appropriate synchronization;
 * the former is limited to working with std::mutex, whereas the latter can work with anything that meets some minimal
 * criteria for being mutex-like, hence the _any suffix. Because std::condition_variable_any is more general, there's the
 * potential for additional costs in terms of size, performance, or operating system resources, so std::condition_variable
 * should be preferred unless the additional flexibility is required.
 *
 * Below scenario describes one way to use condition variable to let the thread that's waiting for work sleep until there's
 * data to process:
 * First off, we have a queue that's used to pass the data between the two threads. When the data is ready, the thread
 * preparing the data locks the mutex protecting the queue using an std::lock_guard and pushes the data onto the queue. It
 * then calls the notify_one() member function on the std::condition_variable instance to notify the waiting thread.
 * On the other side of the fence, we have the processing thread. This thread first locks the mutex, but this time with
 * an std::unique_lock rather than an std::lock_guard.The thread then calls wait() on the std::condition_variable, passing
 * in the lock object and a lambda function that expresses the condition being waited for. The implementation of wait()
 * then checks the condition (by calling the supplied lambda function) and returns if it's satisfied (the lambda function
 * returned true). If the condition isn't satisfied (the lambda function returned false), wait() unlocks the mutex and
 * puts the thread in a blocked or waiting state. When the condition variable is notified by a call to notify_one() from
 * the data-preparation thread, the thread wakes from its slumber (unblocks it), reacquires the lock on the mutex, and
 * checks the condition again, returning from wait() with the mutex still locked if the condition has been satisfied. if
 * the condition hasn't been satisfied, the thread unlocks the mutex and resumes waiting. This is why you need the
 * std::unique_lock rather than the std::lock_guard--the waiting thread must unlock the mutex while it's waiting and lock
 * it again afterward, and std::lock_guard doesn't provide that flexibility. If the mutex remained locked while the thread
 * was sleeping, the data-preparation thread wouldn't be able to lock the mutex to add an item to the queue, and the
 * waiting thread would never be able to see its condition satisfied.
 *
 * During a call to wait(), a condition variable may check the supplied condition any number of times; however it always
 * does so with the mutex locked and will return immediately if (and only if) the function provided to test the condition
 * returns true. When the waiting thread reacquires the mutex and checks the condition, if it isn't in direct response to
 * a notification from another thread, it's called a spurious wake. Because the number and frequency of any such spurious
 * wakes are by definition indeterminate, it isn't advisable to use a function with side effects for the condition check.
 * If you do so, you must be prepared for the side effects to occur multiple times.
 *
 * The flexibility to unlock an std::unique_lock isn't just used for the call to wait(); it's also used once you have the
 * data to process but before processing it. Processing data can potentially be a time-consuming operation, and it's a
 * bad idea to hold a lock on a mutex for longer than necessary.
 *
 * Using a queue to transfer data between threads is a common scenario. Done well, the synchronization can be limited to
 * the queue itself, which greatly reduces the possible number of synchronization problems and race conditions.
 *
 * Condition variables are also useful where there's more than one thread waiting for the same event. If the threads are
 * being used to divide the workload, and thus only one thread should respond to a notification - just run multiple instances
 * of data processing thread. When new data is ready, the call to notify_one() will trigger one of the threads currently
 * executing wait() to check its condition and thus return from wait(). There's no guarantee which thread will be notified
 * or even if there's a thread waiting to be notified; all the processing threads might be still processing data.
 *
 * Another possibility is that several threads are waiting for the same event, and all of them need to respond. This can
 * happen where shared data is being initialized, and the processing threads can all use the same data but need to wait
 * for it to be initialized, or where the threads need to wait for an update to shared data, such as a periodic reinitialization.
 * In these cases, the thread preparing the data can call the notify_all() member function on the condition variable rather
 * than notify_one(). As the name suggests, this causes all the threads currently waiting to check the condition they're
 * waiting for.
 */
template<typename T>
class thread_safe_queue {
private:
    mutable std::mutex mut;     // Mutex must be mutable.
    std::queue<T> data_queue;
    std::condition_variable data_cond;
    public:
    thread_safe_queue() {}
    thread_safe_queue(const thread_safe_queue& other) {
        std::lock_guard<std::mutex> lock(other.mut);
        data_queue = other.data_queue;
    }

    thread_safe_queue& operator=(const thread_safe_queue& ) = delete; // Disallow assignment for simplicity

    void push(T new_value) {
        std::lock_guard lock(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value) {
        std::unique_lock lock(mut);
        data_cond.wait(lock, [&]{ return !data_queue.empty(); });
        value=data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock lock(mut);
        data_cond.wait(lock, [&]{ return !data_queue.empty(); });
        std::shared_ptr<T> result(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return result;
    }

    bool try_pop(T& value) {
        std::lock_guard lock(mut);
        if (data_queue.empty()) return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T>  try_pop() {
        std::lock_guard lock(mut);
        if (data_queue.empty()) return std::shared_ptr<T>(nullptr);
        auto result = std::make_shared<T>(data_queue.front());
        data_queue.pop();
        return result;
    }

    /**
     *
     * Even though empty() is a const member function, and the other parameter to the copy constructor is a const reference,
     * other threads may have non-const references to the object, and be calling mutating member functions, so we still
     * need to lock the mutex. Since locking a mutex is a mutating operation, the mutex object must be marked mutable,
     * so it can be locked in the empty() and in the copy constructor.
     */
    [[nodiscard]] bool empty() const {
        std::lock_guard lk(mut);
        return data_queue.empty();
    }
};