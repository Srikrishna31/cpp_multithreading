//
// Created by coolk on 12-06-2025.
//
#include <future>
#include <iostream>
#include <ostream>

using namespace std::literals;

auto without_packaged_task() {
    std::promise<int> p;
    std::future<int> f = p.get_future();
    auto thr = std::thread{[&p](int a, int b) {
        std::this_thread::sleep_for(2s);
        p.set_value(a + b);
    }, 5, 6};

    std::cout <<"Waiting for result" << std::endl;
    std::cout << f.get() << std::endl;
}

auto main() -> int {
    std::packaged_task<int(int, int)> ptask([](int a, int b) {
        std::this_thread::sleep_for(2s);
        return a + b;
    });

    auto f1 = ptask.get_future();

    std::thread thr{std::move(ptask), 5, 6};

    auto res = f1.get();

    std::cout << res << std::endl;

    thr.join();

    without_packaged_task();

    return 0;
}