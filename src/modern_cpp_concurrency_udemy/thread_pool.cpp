#include "thread_pool.h"
#include <iostream>

thread_pool::thread_pool() {
    auto thread_count = std::thread::hardware_concurrency() - 1;
    std::cout << "Creating a thread pool with " << thread_count << " threads." << std::endl;

    for (decltype(thread_count) i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread{&thread_pool::worker, this});
    }
}

thread_pool::~thread_pool() {
    for(auto& thr: threads) {
        thr.join();
    }
}

auto thread_pool::worker() -> void {
    while (true) {
        Func task;

        work_queue.pop(task);

        task();
    }
}

auto thread_pool::submit(Func func) -> void {
    work_queue.push(std::move(func));
}
