#pragma once

/**
 *    Long running Task
 *      * One task takes a long time to perform
 *           - It cannot execute the following tasks in the queue until this task completes.
 *           - The following tasks will be delayed.
 *      * Other threads have no work to do.
 *           - Inefficient use of resources.
 *
 *   Work Stealing
 *      * A refinement of work sharing
 *      * If a thread's queue is empty, the thread "steals" a task
 *          - The thread takes a task from another thread's queue.
 *      * This ensures that threads are never idle
 *          - Provided there is enough work for all the threads.
 *      * Can perform better than work shaing.
 *          - If some tasks take much longer than others.
 *
 *   Work Stealing Strategy
 *      * If a thread's queue is empty
 *          - Do not wait for a task to arrive on the queue.
 *          - Choose another thread's queue at random.
 *          - If there is a task on that queue, pop it and execute it.
 *          - Otherwise, choose a different thread's queue at random.
 *          - Continue until it finds a task to perform.
 *      * If all queues are empty, pause for a while, and then repeat the process.
 */
#include "concurrent_queue_cv.h"
#include <random>
#include <thread>
#include <functional>

using Func = std::function<void()>;
using Queue = concurrent_queue_cv<Func>;

class thread_pool_cycle_stealing {
    std::unique_ptr<Queue[]> work_queues;

    int pos{0};
    std::mutex pos_mut;

    std::vector<std::thread> threads;

    std::mt19937 mt;

    size_t thread_count;

    auto worker(int idx) -> void;

    auto get_random() -> int;

public:
    thread_pool_cycle_stealing();
    ~thread_pool_cycle_stealing();

    void submit(Func task);
    void wait();
};
