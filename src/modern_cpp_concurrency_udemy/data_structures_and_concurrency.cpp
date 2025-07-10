//
// Created by coolk on 09-07-2025.
//
// Conflicting operations on STL containers are not safe.
// They must be synchronized.
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>

// Shared vector
std::vector<int> vec;

std::mutex m;

auto func1() {
    // Potentially conflicting access - must be protected
    std::lock_guard lgd{m};
    for (int i = 0; i < 100'000; ++i) {
        vec.push_back(i);
    }
}

auto func2() {
    // Potentially conflicting access - must be protected
    std::lock_guard lgd{m};
    for (int i = 100'000; i < 200'000; ++i) {
        vec.push_back(i);
    }
}

/**
 *
 * Modifying Operations
 *      * May affect other parts of the object
 *      * Linked List
 *          # Data stored in a chain of nodes
 *          # Adding or removing an element modifies the surrounding nodes.
 *      * Vector/string/dynamic array
 *          # Data stored in memory block
 *          # Adding or removing elements moves the following elements in memory
 *          # Adding elements may cause the block to be reallocated.
 *      * If other threads are accessing those elements
 *          # Pointers and references may "dangle"
 *          # Iterators may become invalidated.
 *
 *  Basic Thread Safety Guarantee
 *      * STL containers are "memory objects"
 *      * Concurrent reads of the same object are safe
 *      * A single write of an object is safe
 *          # One thread can write to object X
 *          # Provided no other threads access X concurrently
 *      * Concurrent reads and writes of the same object are not safe
 *          # One thread writes to object X
 *          # Other threads must not access X during this operation
 *
 *  Coarse-grained locking
 *      * Locks the entire object
 *          # Easy to do
 *          # Requires no change to the data structure
 *      * Sometimes the only option
 *          # A variable of built-in type
 *          # Types in the C++ standard library
 *          # Types provided by other programmers, if we cannot modify them
 *      * In effect, all code that accesses the object will be single-threaded
 *          # Serial code.
 *
 *  Fine-grained locking
 *      * We can choose which parts of the objects to lock
 *          # We only lock some of the elements
 *      * This is known as "fine-grained locking"
 *          # Allows concurrent access
 *          # Requires writing extra code.
 *          # Requires careful design
 *          # Increases cost of creating an object (mutex initialization)
 */
auto main() -> int {
    std::thread t1{func1};
    std::thread t2{func2};

    t1.join();
    t2.join();

    std::cout << "shptr data: ";
    for (const auto v: vec) {
        std::cout << v << " ";
    }

    std::cout << std::endl << "Finished" << std::endl;

    return 0;
}