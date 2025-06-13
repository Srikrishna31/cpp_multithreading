//
// Created by coolk on 13-06-2025.
//
#include <iostream>
#include <future>
#include <chrono>

auto fibonacci(uint64_t n) -> uint64_t {
    if (n <= 1)
        return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

auto produce() -> int {
    int x = 42;

    using namespace std::literals;
    std::this_thread::sleep_for(2s);

    if (1) {
        throw std::out_of_range("Ooops");
    }

    std::cout << "Produce returning" << std::endl;
    return x;
}

auto main() -> int {
    std::cout << "Calling fibonacci(44)" << std::endl;

    auto result = std::async(fibonacci, 44);

    // Do some other work
    bool finished = false;

    using namespace std::literals;
    while (result.wait_for(1s) != std::future_status::ready) {
        std::cout << "Waiting for the result..." << std::endl;
    }

    std::cout << result.get() << std::endl;

    auto res = std::async(produce);
    std::cout << "Future calling get()..." << std::endl;
    try {
        int x = res.get();
        std::cout << "Future returns from calling get()" << std::endl;
        std::cout << "The answer is " << x << std::endl;
    } catch (std::exception &e) {
        std::cout << "Exception caught " << e.what() << std::endl;
    }

    return 0;
}