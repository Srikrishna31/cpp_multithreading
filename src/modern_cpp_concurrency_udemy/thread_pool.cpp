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
    while (!exit_requested) {
        Func task;

        work_queue.pop(task);

        task();
    }
    if (exit_requested) {
        while (!work_queue.empty()) {
            Func task;
            work_queue.pop(task);
            task();
        }
    }
    std::cout << "Shutting down thread pool." << std::endl;
}

auto thread_pool::submit(Func func) -> void {
    work_queue.push(std::move(func));
}

auto thread_pool::exit(bool exit) -> void {
    exit_requested = exit;
}
