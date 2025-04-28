//
// Created by coolk on 28-04-2025.
//
#include <cassert>
#include <future>

/**
 *
 * Waiting for multiple threads
 *
 * Although std::future handles all the synchronization necessary to transfer data from one thread to another, calls to
 * the member functions of a particular std::future instance are not synchronized with each other. If you access a single
 * std::future object from multiple threads without additional synchronization, you have a data race and undefined behavior.
 * This is by design: std::future models unique ownership of the asynchronous result, and the one-shot nature of get()
 * makes such concurrent access pointless anyway--only one thread can retrieve the value, because, after the first call
 * to get() there's no value left to retrieve.
 *
 * If you need to wait for the same event from more than one thread, you need to use std::shared_future instead. Whereas
 * std::future is only moveable, so ownership can be transferred between instances, but only one instance refers to a
 * particular asynchronous result at a time, std::shared_future instances are copyable, so you can have multiple objects
 * referring to the same associated state.
 *
 * Now, with std::shared_future, member functions on an individual object are still unsynchronized, so to avoid data races
 * when accessing a single object from multiple threads, you must protect accesses with a lock. The preferred way to use
 * it would be to take a copy of the object instead and have each thread access its own copy. Accesses to the shared
 * asynchronous state from multiple threads are safe if each thread accesses that state through its own std::shared_future
 * object.
 *
 * Instances of std::shared_future that reference some asynchronous state are constructed from instances of std::future
 * that reference that state. Since std::future objects don't share ownership of the asynchronous state with any other
 * object, the ownership must be transferred into the std::shared_future using std::move, leaving the std::future in an
 * empty state, as if it was default constructed.
 *
 * Just as with other moveable objects, the transfer of ownership is implicit for rvalues, so you can construct a
 * std::shared_future directly from the return value of the get_future() member function of a std::promise object.
 *
 * std::future has a share() member function that creates a new std::shared_future and transfers ownership to it directly.
 */
int main() {
    std::promise<int> p;
    std::future<int> f = p.get_future();
    assert(f.valid() == true);      // <--- (1) The future is valid
    std::shared_future<int> sf(std::move(f));
    assert(!f.valid());     // <--- (2) f is no longer valid.
    assert(sf.valid() == true);  // <--- (3) sf is now valid.

    std::promise<std::string> ps;
    std::shared_future<std::string> sf1(ps.get_future());   // <--- (3) Implicit transfer of ownership

    assert(sf1.valid() == true);
}