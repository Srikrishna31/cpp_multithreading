//
// Created by coolk on 10-07-2025.
//
#include <memory>
#include <iostream>

// std::shared_ptr has an "atomic" reference counter
std::shared_ptr shptr = std::make_shared<int>(42);

auto func1() {
    // Increments shared_p's reference counter - safe
    std::shared_ptr shp1 = shptr;
}

auto func2() {
    // Increments shared-p's reference counter - safe
}
/**
 *
 *  std::shared_ptr
 *      * When a shared_ptr object is copied or assigned
 *          # There are no memory operations
 *          # Instead, the reference counter is incremented
 *          # When a copy is destroyed, the counter is decremented
 *          # When the last copy is destroyed, the counter is equal to zero
 *          # The allocated memory is then released.
 *  std::shared_ptr Structure
 *      * std::shared_ptr has two private data members
 *      * A pointer to the allocated memory
 *      * A pointer to its "control block"
 *          # The control block contains the reference counter
 *
 *   Threads and std::shared_ptr
 *      * Two potential issues
 *      * The reference counter
 *          # Modified by every copy, assignment, move operation or destructor call
 *          # Conflicting accesses by concurrent threads
 *      * The pointed-to data
 *          # Threads could dereference std::shared_ptr concurrently
 *          # Conflicting accesses
 *      * The reference counter is an atomic type
 *          # This makes it safe to use in threaded programs
 *          # No extra code required when copying, moving or assigning
 *          # Adds extra overhead in single-threaded programs
 *          # Internal synchronization
 *      * The pointed-to data is the responsibility of the programmer
 *          # Must be protected against data races
 *          # Concurrent accesses to the data must be synchronized
 *          # C++20 has std::atomic<std::shared_ptr>
 *          # External synchronization
 */
auto main() -> int {
    std::shared_ptr<int> ptr1{new int(42)};

    auto ptr2 = std::make_shared<int>(42);

    std::cout << *ptr1 << std::endl;

    // Pointer arithmetic is not supported.
    // ++ptr1;

    ptr1 = ptr2;
    std::shared_ptr ptr3{ptr2};
    std::shared_ptr ptr4{ptr3};

    // Releases the allocated memory
    ptr1 = nullptr;

    return 0;
}