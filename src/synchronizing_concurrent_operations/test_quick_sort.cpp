#include <list>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <chrono>

#include "synchronizing_concurrent_operations/sequential_quick_sort.h"
#include "synchronizing_concurrent_operations/parallel_quick_sort.h"

auto generate_random_numbers(const size_t count)-> std::list<int> {
    std::function<std::list<int>(std::list<int>, size_t)> go = [&](std::list<int> state, const size_t curr) -> std::list<int> {
        if (curr >= count) {
            return state;
        }
        state.push_back(std::rand());
        return go(std::move(state), curr + 1);
    };

    return go(std::list<int>(), 0);
};

int main() {
    auto list = generate_random_numbers(1000);
    auto list_copy = list;

    const auto start = std::chrono::high_resolution_clock::now();
    const auto res = sequential_quick_sort(std::move(list));
    const auto stop = std::chrono::high_resolution_clock::now();

    std::cout << "sequential_quick_sort took "
            << std::chrono::duration<double>(stop-start).count()
            << "seconds" << std::endl;

    // for (const int v : res) {
    //     std::cout << v << std::endl;
    // }

    const auto start1 = std::chrono::high_resolution_clock::now();
    const auto res1 = parallel_quick_sort(std::move(list_copy));
    const auto stop1 = std::chrono::high_resolution_clock::now();

    std::cout << "parallel_quick_sort took "
            << std::chrono::duration<double>(stop1-start1).count()
            << "seconds" << std::endl;

    // for (const int v : res1) {
    //     std::cout << v << std::endl;
    // }

    return 0;
}