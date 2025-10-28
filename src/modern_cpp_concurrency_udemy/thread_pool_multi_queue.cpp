#include "thread_pool_multi_queue.h"
#include <iostream>

auto thread_pool_multi_queue::worker(int idx) -> void {
    std::cout << "Worker " << idx << " started" << std::endl;
    while (true) {
        std::function<void()> task;
        work_queues[idx].pop(task);
        task();
    }
    std::cout << "Worker " << idx << " stopped" << std::endl;
}

auto thread_pool_multi_queue::submit(Func func) -> void {
    std::lock_guard<std::mutex> lck_guard{pos_mut};

    // Round robin scheduling
    work_queues[pos].push(func);

    pos = (pos + 1) % threads.size();
}


thread_pool_multi_queue::thread_pool_multi_queue() {
    auto thread_count = std::thread::hardware_concurrency() - 1;
    std::cout << "Creating a thread pool with " << thread_count << " threads" << std::endl;

    work_queues = std::make_unique<Queue[]>(thread_count);

    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(&thread_pool_multi_queue::worker, this, i);
    }
}

thread_pool_multi_queue::~thread_pool_multi_queue() {
    for (auto& thread : threads) {
        thread.join();
    }
}
