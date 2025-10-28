#include "thread_pool.h"
#include "thread_pool_multi_queue.h"
#include "thread_pool_cycle_stealing.h"
#include <iostream>
#include <chrono>

using namespace std::literals;

auto task() -> void {
    std::cout << "Thread id: " << std::this_thread::get_id() << " starting a task" << std::endl;
    std::this_thread::sleep_for(100ms);
    std::cout << "Thread id: " << std::this_thread::get_id() << " finishing a task" << std::endl;
}

void task2() {
    std::cout << "Thread id: " << std::this_thread::get_id() << " starting a task" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "Thread id: " << std::this_thread::get_id() << " finishing a task" << std::endl;
}
auto main() -> int {
    // thread_pool pool;
    // thread_pool_multi_queue pool;
    thread_pool_cycle_stealing pool;

    pool.submit(task2);
    for (int i = 0; i < 200; ++i) {
        pool.submit(task);
    }

    pool.submit([&pool](){
        std::this_thread::sleep_for(1s);
        std::cout << "All tasks finished" << std::endl;
    });

    std::this_thread::sleep_for(5s);
    // pool.exit(true);

    return 0;
}
