#pragma once
/**
 * Queue
 *  - FIFO Data Structure
 *  - Stores elements in the order they were inserted
 *      * Elements are "pushed" onto the back of the queue
 *      * Elements are "popped" off the front
 *
 *  - std::queue limitations
 *      * Implemented as a "memory location"
 *      * If pop() is called on an empty container, the behavior is undefined.
 *      * Removing an element involves two operations:
 *          * front() returns a reference to the element at the front of the queue
 *          * pop() removes the element at the front, without returning anythin.
 *      * Intended to provide exception safety
 *          * But removes thread safety
 *          * Race condition between front() and pop()
 */

 #include <queue>
 #include <mutex>

 class concurrent_queue_empty: public std::runtime_error {
     public:
         concurrent_queue_empty() : std::runtime_error("Queue is empty") {}
         concurrent_queue_empty(const char* s): std::runtime_error(s) {}
 };

 class concurrent_queue_full: public std::runtime_error {
     public:
         concurrent_queue_full() : std::runtime_error("Queue is full") {}
         concurrent_queue_full(const char* s): std::runtime_error(s) {}
         concurrent_queue_full(const std::string& s): std::runtime_error(s) {}
 };

 template <typename T>
 class concurrent_queue {
     std::mutex mut;
     std::queue<T> queue;
     int max{50};

public:
    concurrent_queue() = default;
    concurrent_queue(int max):max(max) {};

    concurrent_queue(const concurrent_queue&) = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;
    concurrent_queue& operator=(concurrent_queue&&) = delete;
    concurrent_queue(concurrent_queue&&) = delete;

    auto push(T value) -> void {
        std::lock_guard<std::mutex> lck_guard(mut);
        if (queue.size() > max) {
            throw concurrent_queue_full();
        }

        queue.push(value);
    }

    auto pop(T& value) -> void {
        std::unique_lock<std::mutex> uniq_lck(mut);
        if (queue.empty()) {
            throw concurrent_queue_empty();
        }

        value = queue.front();
        queue.pop();
    }
 };
