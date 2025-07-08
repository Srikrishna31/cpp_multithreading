//
// Created by coolk on 03-07-2025.
//
#include <algorithm>
#include <numeric>
#include <iostream>
#include <vector>
#include <execution>

namespace se = std::execution;

template <typename T>
auto print_vector(const std::vector<T>& vec) {
    std::cout << "Vector elements: ";
    std::ranges::for_each(vec,[](const auto& v) {std::cout << v << " ";});
    std::cout << std::endl;
}

auto transform_reduce_example() {
    using namespace std;

    auto expected = std::vector{0.1, 0.2, 0.3, 0.4, 0.5};
    auto actual = std::vector{0.09, 0.22, 0.27, 0.41, 0.52};

    const auto max_diff = std::transform_reduce(se::par, begin(expected), end(expected), begin(actual),
                    // Initial value for the largest difference
                    0.0,
                    // "Reduce" Operation
                    [](auto diff1, auto diff2) { return std::max(diff1, diff2); },
                    // "Transform" Operation
                    [](auto exp, auto act) {return std::abs(act-exp);}
                    );

    print_vector(expected);
    print_vector(actual);
    std::cout << "Max difference is: " << max_diff << std::endl;
}
/**
 *  std::accumulate() Execution
 *      * Specified to execute sequentially
 *      * Cannot be parallelized
 *          # Each operation must complete before the next one can start
 *      * Cannot be vectorized
 *          # Each operation can only take two arguments.
 *
 *  std::reduce() Execution
 *      * Re-implementation of std::accumulate()
 *      * Supports execution policies
 *      * Reduce with parallel execution resembles a tree of operations.
 *
 *  std::reduce() Restrictions
 *      * reduce() will not give the correct answer if
 *          # Reordering the operations alters the result, or
 *          # Regrouping them alters the result
 *      * The operator used must be
 *          # Commutative - order of operations is not important x+y == y+x
 *          # Associative - grouping operations does not change the result (x+y) + z = x + (y + z)
 *      * Not true for subtraction, floating-point arithmetic, etc.
 *
 *  std::partial_sum()
 *      * Calculates the sum of the elements so far
 *      * As with std::accumulate, the calculation must be done in a fixed order
 *
 *  std::inclusive_scan()
 *      * Works the same way as std::partial_sum
 *      * We can optionally add an execution policy
 *  std::exclusive_scan()
 *      * Similar to inclusive_scan, but excludes the current element
 *      * Takes an extra argument
 *      * Uses it instead of the current element
 *
 *  std::transform()
 *      * Takes an iterator range and a callable object
 *      * Applies the callable object to every element in the range
 *      * Stores the result in a destination
 *      * Similar to "map" in functional languages
 *  Binary overload of std::transform()
 *      * std::transform() can take an extra argument
 *          # This represents a second source
 *          # The function object is now a binary operator
 *          # Applied to corresponding pairs of elements from each source
 *          # The result is stored in the destination
 *  Transform and Reduce Pattern
 *      * A very common pattern in parallel programming
 *      * Also known as "map and reduce"
 *          # Divide the data into subsets
 *          # Start a thread for each subset
 *          # Each thread calls transform()
 *          # transform() performs some operation on the thread's subset
 *          # Call reduce() to combine each thread's results into the final answer.
 *          # Naive approach
 *              - Start up some threads which call transform()
 *              - Call reduce()
 *          # Using separate algorithm calls slows things down
 *              - Each transform() thread has to store its result
 *              - reduce() cannot start until all the transform() threads have finished
 *              - reduce() has to read the results from the transform() threads
 *              - reduce() may start up its own threads
 *          # std::transform_reduce()
 *              - Combines the two functions: Avoids the overhead of separate transform() and reduce() calls
 *              - Is a reimplementation of std::inner_product() with support for execution policies
 *     * std::inner_product()
 *          # Multiplies the corresponding elements of two containers together
 *          # Calculates the sum of these products.
 *
 *  Disadvantages of Execution Policies
 *      * May not have any effect
 *          # Not supported on some compilers
 *          # Not fully implemented on all compilers
 *          # May fall back to non-policy version
 *      * Extra overhead
 *          # The algorithm may start up new threads
 *          # The algorithm must manage these threads
 *  When to use an execution policy
 *      * Do not use an execution policy if
 *          # Your code has to be portable to other compilers
 *          # The task is essentially sequential
 *          # Operation order is important
 *          # The algorithm call throws exceptions
 *          # Unless immediate termination is acceptable
 *          # Preventing data races costs more than not using an execution policy
 *      * Consider using an execution policy if
 *          # Measurement shows a worthwhile improvement in performance
 *          # It can be done safely and correctly
 *
 *  Which Execution policy to use?
 *      * Sequenced execution is mainly used for debugging
 *          # Same as non-policy, but
 *          # Allows out of order execution
 *          # Terminates on exceptions
 *      * Parallel unsequenced execution
 *          # Has the most potential to improve performance
 *          # Also has the strictest requirements. Use when
 *              - Data races cannot occur
 *              - Code does not modify shared state
 *      * Parallel execution
 *          # Use when vectorization is not safe
 *          # Data races cannot occur, but code modifies shared state
 *      * Unsequenced execution (C++20)
 *          # Can be used with single threading
 *          # If code does not modify shared state.
 *
 */
auto main() -> int {
    auto vec = std::vector{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    print_vector(vec);

    const auto sum = std::accumulate(vec.begin(), vec.end(), 0);
    std::cout << "Sum of all elements: " << sum << std::endl;

    // Sum the elements of vec, using initial value as 0.
    // Performed as 8 parallel additions
    // ((0+1) + (2+3) + (4+5) + (6+7) + (8+9) + (10+11) + (12+13) + (14+15))
    const auto sum1 = std::reduce(se::par, vec.cbegin(), vec.cend(), 0);
    std::cout << "Sum of all elements: " << sum1 << std::endl;

    auto vec1 = std::vector<decltype(vec)::value_type>(vec.size());
    std::partial_sum(vec.cbegin(), vec.cend(), vec1.begin());
    print_vector(vec1);

    auto vec2 = std::vector<decltype(vec)::value_type>(vec.size());
    std::inclusive_scan(se::par, vec.cbegin(), vec.cend(), vec2.begin());
    print_vector(vec2);


    auto vec3 = std::vector<decltype(vec)::value_type>(vec.size());
    std::exclusive_scan(se::par, vec.cbegin(), vec.cend(), vec3.begin(), -1);
    print_vector(vec3);

    auto vec4 = std::vector<decltype(vec)::value_type>{};
    std::transform(vec.cbegin(), vec.cend(),std::back_inserter(vec4),[](int n) { return 2*n;});
    print_vector(vec4);

    const auto vec5 = std::vector<decltype(vec)::value_type>{17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    print_vector(vec5);
    auto vec6 = std::vector<decltype(vec)::value_type>{};
    std::transform(vec.cbegin(), vec.cend(),vec5.cbegin(), std::back_inserter(vec6),[](int a, int b) { return a+b;});
    print_vector(vec6);

    const auto vec7 = std::vector<decltype(vec)::value_type>{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    const auto vec8 = std::vector<decltype(vec)::value_type>{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};

    const auto res = std::inner_product(vec7.cbegin(), vec7.cend(), vec8.cbegin(), 0);

    print_vector(vec7);
    print_vector(vec8);
    std::cout << res << std::endl;

    const auto res1 = std::transform_reduce(se::par, vec7.cbegin(), vec7.cend(), vec8.begin(), 0);
    std::cout << res1 << std::endl;

    transform_reduce_example();

    return 0;
}