//
// Created by coolk on 02-06-2025.
//
// Example using std::promise and std::future
// to throw an exception from a producer thread to a consumer thread
#include <future>
#include <iostream>
#include <thread>
#include <exception>
#include <chrono>

// auto produce(std::promise<int>& promise) {
//     try {
//         using namespace std::literals;
//         int x = 42;
//         std::this_thread::sleep_for(2s);
//
//         // Code that may throw
//         if (1)
//                 throw std::out_of_range("Ooops");
//
//         std::cout << "Promise sets shared state to " << x << std::endl;
//         promise.set_value(x);
//     } catch (...) {
//         promise.set_exception(std::current_exception());
//     }
// }

auto produce(std::promise<int>& promise) {
    using namespace std::literals;
    int x = 42;
    std::this_thread::sleep_for(2s);

    if (0) {
        promise.set_exception(std::make_exception_ptr(std::out_of_range("Ooops")));
        return;
    }

    std::cout << "Promise sets shared state to " << x << std::endl;
    promise.set_value(x);
}

auto consume(std::future<int>& future) {
    std::cout << "Future calling get()..." << std::endl;
    try {
        int x = future.get();
        std::cout << "Future returns from calling get()" << std::endl;
        std::cout << "The answer is " << x << std::endl;
    } catch (std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
}
/**
 * Futures and promises
 *      * std::future and std::promise
 *          # Classes for transferring data between threads.
 *          # Together, these set up a "shared state" between threads
 *          # The shared state can transfer from one thread to another
 *              . No shared data variables
 *              . No explicit locking
 *      * Producer-Consumer Model
 *          # Futures and promises uses a producer-consumer model
 *              . Reader/writer threads are an example of this model
 *          # A "producer" thread will generate a result
 *          # A "consumer" thread waits for the result.
 *          # The producer thread generates the result.
 *          # The producer thread stores the result in the shared state.
 *          # The consumer thread reads the result from the shared state.
 *
 *     * Transfer of Data using Future and Promise
 *          # An std::promise is associated with the producer.
 *          # An std::future object is associated with the consumer
 *              . The consumer calls a member function of the future object.
 *              . The function blocks until the result becomes available.
 *          # The producer thread sends the result
 *              . The promise object stores the result in the shared state.
 *          # The consumer thread receives the result
 *              . The member function reads the result from the shared state
 *              . The member function returns the result.
 *          # Exception Handling
 *              . Futures and promises also work with exceptions
 *                  > The promise stores the exception in the shared state
 *              . This exception will be rethrown in the consumer thread
 *                  > By the future's blocking function
 *              . The producer thread "throws" the exception to the consumer.
 *         # Future and Promise
 *              . A promise object is associated with a future object.
 *              . Together they create a "shared state"
 *                  > The promise object stores the result in the shared state.
 *                  > The future object gets the result from the shared state.
 *
 *              . std::future
 *                  > Represents a result that is not yet available.
 *                  > One of the most important classes in C++ concurrency
 *                      - Works with many different asynchronous objects and operations
 *                      - Not just std::promise
 *                  > An std::future object is not usually created directly
 *                      - Obtained from an std::promise object.
 *                      - Or returned by an asynchronous operation.
 */
auto main() -> int {
    std::promise<int> p;

    std::future<int> f = p.get_future();

    std::thread thr_producer(produce, std::ref(p));
    std::thread thr_consume(consume, std::ref(f));

    thr_producer.join();
    thr_consume.join();
}