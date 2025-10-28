#pragma once

/**
 *    Thread pool with Multiple Queues
 *      - The queue can become a bottle-neck
 *         * When a thread takes a task off the queue, it locks the queue.
 *         * Other threads are blocked until this operation is complete.
 *         * If there are many small tasks, this can affect performance.
 *
 *      - An alternative is to use a separate queue for each thread.
 *         * A thread never has to wait to get its next task.
 *         * Uses more memory.
 *      - Work sharing
 *         * Can perform better than a single queue pool, when there are many small tasks.
 *         * If a thread's queue is empty, the thread is idle.
 *       - Work sharing implementation
 *          * Replace the queue by a fixed-size vector of queues, one element for each thread.
 *          * "Round-robin" scheduling
 *             * Put a new task on the next thread's queue.
 *             * After the last element of the vector, go to the front element.
 */

#include "concurrent_queue_cv.h"
#include <thread>
#include <functional>

// All the task functions will have this type.
using Func = std::function<void()>;
using Queue = concurrent_queue_cv<Func>;

class thread_pool_multi_queue {
    // Each thread has its own queue of task functions
    std::unique_ptr<Queue[]> work_queues;

    // Index into the vector of queues
    int pos{0};
    std::mutex pos_mut;

    // Vector of thread objects which make up the pool
    std::vector<std::thread> threads;

    // Entry point function for the threads
    auto worker(int idx) -> void;

    bool exit_requested = false;

public:
   thread_pool_multi_queue();
   ~thread_pool_multi_queue();

   // Add a task to the queue.
   auto submit(Func func) -> void;

   // Exit the thread pool
   auto exit(bool exit) -> void;

};
