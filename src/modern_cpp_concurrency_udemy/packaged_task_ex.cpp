//
// Created by coolk on 12-06-2025.
//
#include <future>
#include <iostream>
#include <ostream>

using namespace std::literals;

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

    return 0;
}