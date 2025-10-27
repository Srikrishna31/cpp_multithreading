#include "thread_pool.h"
#include <iostream>
#include <chrono>

using namespace std::literals;

auto task() -> void {
    std::cout << "Thread id: " << std::this_thread::get_id() << " starting a task" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Thread id: " << std::this_thread::get_id() << " finishing a task" << std::endl;
}

auto main() -> int {
    thread_pool pool;

    for (int i = 0; i < 20; ++i) {
        pool.submit(task);
    }

    pool.submit([&pool](){
        std::this_thread::sleep_for(1s);
        std::cout << "All tasks finished" << std::endl;
    });

    return 0;
}
