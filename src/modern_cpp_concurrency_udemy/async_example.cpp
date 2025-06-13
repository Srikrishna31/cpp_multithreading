//
// Created by coolk on 13-06-2025.
//
#include <iostream>
#include <future>
#include <chrono>

using namespace std::literals;

auto fibonacci(uint64_t n) -> uint64_t {
    if (n <= 1)
        return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

auto produce() -> int {
    int x = 42;

    std::this_thread::sleep_for(2s);

    if (1) {
        throw std::out_of_range("Ooops");
    }

    std::cout << "Produce returning" << std::endl;
    return x;
}

auto task() -> int {

    std::cout << "Executing task() in thread with ID: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "Returning from task()" << std::endl;
    return 42;
}

auto func(const std::string& option = "default"s) {
    auto result = std::future<int>{};

    if (option == "async"s)
        result = std::async(std::launch::async, task);
    else if (option == "defer"s)
        result = std::async(std::launch::deferred, task);
    else
        result = std::async(task);

    std::cout << "Calling async with option: \"" << option << "\"" << std::endl;
    std::this_thread::sleep_for(2s);
    std::cout << "Calling get()" << std::endl;
    std::cout << "Task result: " << result.get() << std::endl;
}

/**
 *
 * Launch Options
 *  > std::async() may start a new thread for the new task
 *  > Or it may run the task in the same thread
 *  > Launch flags
 *      # std::launch::async
 *          * A new thread is started for the task
 *          * The task is executed as soon as the thread starts
 *      # std::launch::deferred
 *          * Nothing happens until get() is called on the returned future
 *          * The task is then executed("lazy evaluation")
 *      # If both flags are set
 *          * The implementation decides whether to start a new thread
 *          * This is the default
 *
 *   Launch Policy Recommendations
 *      # Use the async launch option if any of these are true
 *          * The task must execute in a separate thread
 *          * The task must start immediately
 *          * The task will use thread-local storage
 *          * The task function must be executed, even if get() is not called
 *          * The thread receiving the future will call wait_for() or wait_until()
 *      # Use the deferred launch option if
 *          * The task must be run in the thread which calls get()
 *          * The task must be executed, even if no more threads can be created.
 *          * You want lazy execution of the task.
 *      # Otherwise, let the implementation choose.
 *
 *   Choosing a Thread Object
 *      # Create an std::thread object
 *      # Create an std::packaged_task object
 *      # Call std::async()
 *          * Advantages of std::async()
 *              > The simplest way to execute a task
 *                  - Easy to obtain the return value from a task
 *                  - Or to catch any exception thrown in the task
 *                  - Choice of running the task synchronously or asynchronously
 *              > Higher-level abstraction than std::thread
 *                  - The library manages the threads for the programmer
 *                  - And the inter-thread communication
 *                  - No need to use shared data
 *         * Advantages of std::packaged_task
 *              > The best choice if we want to represent tasks as objects.
 *              > A lower-level abstraction than std::async()
 *                  - Can control when a task is executed
 *                  - Can control on which thread it is executed.
 *         * Advantages of std::thread
 *              > The most flexible
 *                  - Allows access to the underlying software thread
 *                  - Useful for features not supported by standard C++
 *                  - Can be detached.
 */
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

    std::cout << "In main thread with ID: " << std::this_thread::get_id() << std::endl;

    func("async");
    func("deferred");
    func("default");
    return 0;
}