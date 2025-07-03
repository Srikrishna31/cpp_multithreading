//
// Created by coolk on 01-07-2025.
//
#include <algorithm>
#include <vector>
#include <iostream>
#include <execution>

namespace se = std::execution;

auto print_container(const std::vector<int>& vec) -> void {
    std::ranges::for_each(vec,[](const auto& a) { std::cout << a << " "; });
    std::cout << std::endl;
}

auto exception_sort() {
    auto vec = std::vector{3,1,4,1,5,9};

    try {
        // std::ranges::sort(vec, [](int a, int b) {
        //     throw std::out_of_range("Oops");
        //     return true;
        // });
        std::sort(se::seq, vec.begin(), vec.end(), [](int a, int b) {
            throw std::out_of_range("Oops");
            return true;
        });
    } catch (std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    print_container(vec);
}
/**
 *  Execution Policies
 *   > Code Execution
 *      * Modern computers support four different ways:
 *          # Sequential
 *              - A single instruction processes one data item
 *          # Vectorized
 *              - A single instruction processes several data items
 *              - Requires suitable data structure and hardware support
 *          # Parallelized
 *              - Several instructions each process one data item, at the same time
 *              - Requires suitable algorithm
 *          # Parlallelized + vectorized
 *              - Several instructions each process several data items, at the same time
 *              - Requires suitable algorithm, data structure and hardware support.
 *   > C++ 17 Execution Policies
 *      * Choice of "execution policies" for an algorithm call
 *          # Sequenced execution
 *          # Parallel execution
 *          # Parallel and vectorized execution
 *          # Vectorized execution only (not until C++20)
 *      * Not supported by all compilers (possibly now, should be supported)
 *      * "Request" which may be ignored
 *          # If parallel and/or vectorized execution is not supported
 *          # If insufficient system resources are available
 *          # If parallel and/or vectorized version has not been implemented
 *   > Sequenced Execution
 *      * All operations are performed on a single thread
 *          # The thread which calls the algorithm
 *      * Operations will not be interleaved.
 *          # May not necessarily be executed in a specific order.1
 *   > Parallel Execution
 *      * Operations performed in parallel across a number of threads
 *          # May include the thread which calls the algorithm
 *      * Guarantees
 *          # An operation will run on the same thread for its entire duration
 *          # Operations performed on the same thread will not be interleaved
 *          # But may not necessarily be executed in a specific order
 *      * Cautions
 *          # Operations performed on different threads may interleave
 *          # The programmer must prevent data races
 *    > Unsequenced Execution (C++20)
 *      * Operations are performed on a single thread
 *          # The thread which calls the algorithm
 *      * Guarantees
 *          # Operations will not be interleaved
 *      * Cautions
 *          # Operations may not necessarily be executed in a specific order
 *          # The programmer must avoid any modification of shared state between elements or between threads
 *          # Memory allocation and deallocation
 *          # Mutexes, locks and other forms of synchronization
 *    > Parallel Unsequenced Execution
 *      * Operations performed in parallel across a number of threads
 *          # May include the thread which calls the algorithm
 *      * Cautions
 *          # Operations performed on the same thread may be interleaved
 *          # They may not necessarily be executed in a specific order
 *          # An operation may be migrated from one thread to another
 *          # The programmer must avoid data races
 *          # The programmer must avoid any modification of shared state between elements or between threads.
 *    > Numeric Algorithms and Execution Policies
 *      * Some algorithms in <numeric> now have two versions
 *          # The C++14 version
 *          # A new version with policy support
 *      * The new versions have new names
 *          # accumulate() -> reduce()
 *          # partial_sum() -> inclusive_scan(), exclusive_scan()
 *      * There is also a new "fused" algorithm with policy support
 *          # transform() + inner_product() -> transform_reduce()
 *    > Algorithms and Exceptions in C++ 14
 *      * Algorithms can throw exceptions
 *          # e.g. An algorithm call which applies a function to every element
 *          # The function throws an exception
 *      * The exception will be handled by other code.
 *          # If there is no handler, execution ends.
 *      * This approach doesn't work with execution policies
 *          # Maybe multiple threads
 *          # Each thread has its own execution stack
 *      * If an exception is thrown, std::terminate() is called.
 */
auto main() -> int {
    auto vec = std::vector{3,1,4,1,5,9};
    auto greater_than = [] (int a, int b) { return b < a; };
    // Sequential execution
    std::sort(se::seq, vec.begin(), vec.end(),greater_than);

    print_container(vec);

    auto vec1 = std::vector<int>(20'000);
    int count = 0;

    //Data race
    std::for_each(se::par, vec1.begin(), vec1.end(),[&count](int& x) {x = ++count;});

    print_container(vec1);

    // Unsequential
    auto vec2 = std::vector{3,1,4,1,5,9};
    std::sort(se::unseq, vec2.begin(), vec2.end(),greater_than);

    print_container(vec2);

    // parallel unsequential
    auto vec3 = std::vector{3,1,4,1,5,9};
    std::sort(se::par_unseq, vec3.begin(), vec3.end(),greater_than);

    print_container(vec3);

    exception_sort();

    std::cout << "Exiting gracefully" << std::endl;
    return 0;
}

