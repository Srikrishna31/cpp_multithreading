//
// Created by coolk on 28-06-2025.
//
#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <future>
#include <random>

using namespace std;

static std::mt19937 mt;
uniform_real_distribution<double> dist{0, 100};

auto accum(double *beg, double *end) -> double {
    return std::accumulate(beg, end, 0.0);
}

// Divide the data into 4 parts
// Use a separate part to process each subset
auto add_parallel(std::vector<double>& vec) -> double {
    auto vec0 = &vec[0];
    auto vsize = vec.size();

    // Pass the subset's range as argument to the task function
    auto fut1 = std::async(std::launch::async, accum, vec0, vec0 + vsize/4);
    auto fut2 = std::async(std::launch::async, accum, vec0 + vsize/4, vec0 + 2*vsize/4);
    auto fut3 = std::async(std::launch::async, accum, vec0 + 2*vsize/4, vec0 + 3*vsize/4);
    auto fut4 = std::async(std::launch::async, accum, vec0 + 3*vsize/4, vec0 + vsize);

    //Reduce Step
    // Combine the results for each subset
    return fut1.get() + fut2.get() + fut3.get() + fut4.get();
}

auto add_parallel_packaged(std::vector<double>& vec) -> double {
    using task_type = double(double*, double*);

    auto ptask1 = std::packaged_task{accum};
    auto ptask2 = std::packaged_task{accum};
    auto ptask3 = std::packaged_task{accum};
    auto ptask4 = std::packaged_task{accum};

    auto fut1 = ptask1.get_future();
    auto fut2 = ptask2.get_future();
    auto fut3 = ptask3.get_future();
    auto fut4 = ptask4.get_future();

    const auto vec0 = &vec[0];
    const auto vsize = vec.size();

    std::thread t1{std::move(ptask1), vec0, vec0 + vsize/4};
    std::thread t2{std::move(ptask2), vec0 + vsize/4, vec0 + 2*vsize/4};
    std::thread t3{std::move(ptask3), vec0 + 2*vsize/4, vec0 + 3*vsize/4};
    std::thread t4{std::move(ptask4), vec0 + 3*vsize/4, vec0 + vsize};

    t1.join(); t2.join(); t3.join(); t4.join();

    return fut1.get() + fut2.get() + fut3.get() + fut4.get();
}

/**
 * Concurrency vs Parallelism
 *  # Concurrecny
 *      - Describes conceptually distinct tasks
 *          > Separation of concerns
 *          > Can run on a single core.
 *      - These tasks often interact
 *          > Wait for an event
 *          > Wait for each other
 *      - The tasks often overlap in time
 *      - Concurrency is a feature of the program structure.
 *
 *  # Parallelism
 *      - The tasks are identical
 *      - They all run at the same time
 *          > Run on multiple cores to improve scalability
 *      - These tasks run independently of each other
 *      - Parallelism is a feature of the algorithm being run
 *      - Variants of Parallelism
 *          > Task Parallelism
 *              * Distributed Processing
 *                  (.) Also known as "Thread-level Parallelism (TLP)"
 *              * Split a large task into smaller tasks
 *          > Data Parallelism
 *              * Distributed Data
 *                  (.) A data set is divided up into several subsets
 *                  (.) Process all the subsets concurrently
 *              * Each thread works on one of the subsets
 *              * A final reduce step
 *                  (.) Collects the result for each subset
 *                  (.) Combines the partial results into the overall result.
 *              * Also known as "vector processing" or vectorization
 *                  (.) Used in Graphics Processor Units(GPUs)
 *              * Modern CPUs have support for vectorization
 *                  (.) A single instruction can operate on multiple arguments
 *                  (.) Known as Single Instruction/Multiple Data architecture or "SIMD"
 *                  (.) x86 provided SSE family which performs 128-bit SIMD instructions
 *                  (.) Superseded by 256-bit AVX family
 *              * Locality of Data References
 *                  (.) Data parallelism can improve data locality
 *                  (.) Each core processes 1/Nth of the data
 *                  (.) All the data that each core needs is in cache
 *                      < No fetches from RAM
 *                      < No interaction with cache controller
 *          > Pipelining
 *              * Dependent Tasks
 *                  (.) Task B requires output from Task A, task C requires output from B
 *                  (.) B cannot start until A has completed and produced its output
 *              * Perform dependent tasks sequentially
 *              * Process data concurrently
 *          > Graph parallelism
 *              * Similar to pipelining, but with an arbitrary graph of dependencies.
 */
auto main() -> int {

    std::vector<double> vec(16);
    std::iota(vec.begin(), vec.end(), 1.0);

    std::vector<double> vrand(10'000);
    std::generate(vrand.begin(), vrand.end(), [&vrand](){return dist(mt); });

    std::cout << "Sum of first 16 integers: " << add_parallel(vec) << std::endl;
    std::cout << "Sum of 10,000 random numbers: " << add_parallel(vrand) << std::endl;

    std::cout << "Sum of first 16 integers: " << add_parallel_packaged(vec) << std::endl;
    std::cout << "Sum of 10,000 random numbers: " << add_parallel_packaged(vrand) << std::endl;
    return 0;
}