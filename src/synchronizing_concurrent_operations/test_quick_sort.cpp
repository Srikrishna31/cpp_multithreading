#include <list>
#include <cstdlib>
#include <iostream>
#include <functional>

#include "synchronizing_concurrent_operations/sequential_quick_sort.h"
#include "synchronizing_concurrent_operations/parallel_quick_sort.h"

auto generate_random_numbers(const size_t count)-> std::list<int> {
    std::function<std::list<int>(std::list<int>, size_t)> go = [&](std::list<int> state, const size_t curr) -> std::list<int> {
        if (curr == count) {
            return state;
        }
        state.push_back(std::rand());
        return go(std::move(state), curr + 1);
    };

    return go(std::list<int>(), 0);
};

int main() {
    const auto list = generate_random_numbers(100);

    for (const int v : list) {
        std::cout << v << std::endl;
    }
    return 0;
}