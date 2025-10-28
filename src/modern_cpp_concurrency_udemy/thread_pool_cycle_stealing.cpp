#include "thread_pool_cycle_stealing.h"
#include <iostream>

// Protect the shared state in random number generator.
std::mutex rand_mut;

thread_pool_cycle_stealing::thread_pool_cycle_stealing() {
    thread_count = std::thread::hardware_concurrency() - 1;
    std::cout << "Creating a thread pool with " << thread_count << " threads" << std::endl;

    work_queues = std::make_unique<Queue[]>(thread_count);

    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(&thread_pool_cycle_stealing::worker, this, i);
    }
}

thread_pool_cycle_stealing::~thread_pool_cycle_stealing() {
    for (auto& thread : threads) {
        thread.join();
    }
}

auto thread_pool_cycle_stealing::get_random() -> int {
    std::lock_guard<std::mutex> lock(rand_mut);
    std::uniform_int_distribution<int> dist(0, thread_count - 1);

    return dist(mt);
}


auto thread_pool_cycle_stealing::worker(int idx) -> void {
    while (true) {
        int visited = 0;
        Func task;

        int i = idx;

        while(!work_queues[i].try_pop(task)) {
            // Nothing on this queue
            i = get_random();

            // Hot loop avoidance
            // If we have checked "enough" queues, pause for a while, then start again with our own queue.
            if (++visited == thread_count) {
                std::this_thread::yield();
                visited = 0;
                i = idx;
            }
        }

        task();
    }
}

auto thread_pool_cycle_stealing::submit(Func func) -> void {
    int i;

    // Pick a random queue
    do {
        i = get_random();
    }
    // Until we find one that is not full
    while(!work_queues[i].try_push(func));
}
