//
// Created by coolk on 06-04-2025.
//
#include <atomic>
#include <thread>
#include <cassert>

std::atomic<int> data[5];
std::atomic<bool> sync1(false), sync2(false);

void thread_1() {
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync1.store(true, std::memory_order_release);   // <--- (1) Set sync1
}

void thread_2() {
    while (!sync1.load(std::memory_order_acquire)) {}  // <--- (2) Loop until sync1 is set.
    sync2.store(true, std::memory_order_release);   // <--- (3) Set sync2
}

void thread_3() {
    while (!sync2.load(std::memory_order_acquire)) {}   // <--- (4) Loop until sync2 is set
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
}

/**
 *         Transitive synchronization with acquire-release ordering
 * One of the important properties of "inter-thread happens-before" is that it's transitive: if A inter-thread happens-before
 * B and B inter-thread happens-before C, then A inter-thread happens-before C. This means that acquire-release ordering
 * can be used to synchronize data across several threads, even when the "intermediate" threads haven't actually touched
 * the data.
 *
 * In order to think about transitive ordering, you need at least three threads. The first thread modifies some shared
 * variables and does a store-release to one of them. A second thread then reads the variable subject to the store-release
 * with a load-acquire and performs a store-release on a second shared variable. Finally, a third-thread does a load-acquire
 * on that second shared variable. Provided that the load-acquire operations see the values written by the store-release
 * operations to ensure the synchronizes-with relationships, this third thread can read the values of the other variables
 * stored by the first thread, even if the intermediate thread didn't touch any of them.
 *
 * Even though thread_2 only touches the variables sync1 (2) and sync2 (3), this is enough for synchronization between
 * thread_1 and thread_3 to ensure that the asserts don't fire. First off, the stores to data from thread_1 happens-before
 * the store to sync1 (1), because they're sequenced-before it in the same thread. Because the load from sync1 (2) is in a
 * while loop, it will eventually see the value stored from thread_1 and thus form the second half of the release-acquire
 * pair. Therefore, the store to sync1 happens-before the final load from sync1 in the while loop. This load is sequenced-before
 * (and thus happens-before) the store to sync2 (3), which forms a release-acquire pair with the final load from the while
 * loop in thread_3 (4). The store to sync2 (3) thus happens-before the load (4), which happens-before the loads from the
 * data. Because of the transitive nature of happens-before, you can chain it all together: the store to data happen-before
 * the store to sync1 (1), which happens-before the load from sync1 (2), which happens-before the store to sync1 (1), which
 * happens-before the load from sync1 (2), which happens-before the store to sync2 (3), which happens-before the load
 * from sync2 (4), which happens-before the loads from data. Thus the stores to data  in thread_1 happen-before the loads
 * from data in thread_3, and the asserts can't fire.
 *
 * If you use read-modify-write operations, it's important to pick which semantics you desire. A fetch_sub operation with
 * memory_order_acquire semantics doesn't synchronize-with anything even though it stores a value, because it isn't a
 * release operation. Likewise, a store can't synchronize with a fetch_or with memory_order_release semantics, because the
 * read part of the fetch_or isn't an acquire operation. Read-modify-write operations with memory_order_acq_rel semantics
 * behave as both an acquire and a release, so a prior store can synchronize-with such an operation, and it can synchronize-with
 * a subsequent load.
 *
 * If you mix acquire-release operations with sequentially consistent operations, the sequentially consistent loads behave
 * like loads with acquire semantics, and sequentially consistent stores behave like store with release semantics.
 * Sequentially consistent read-modify-write operations behave as both acquire and release operations. Relaxed operations
 * are still relaxed but are bound by the additional synchronizes-with and consequent happens-before relationships
 * introduced through the use of acquire_release semantics.
 *
 * Locking a mutex is an acquire operation, and unlocking the mutex is a release operation. With mutexes, you learn that
 * you must ensure that the same mutex is locked when you read a value as was locked when you wrote it, and the same
 * applies to atomic variables: The acquire and release operations have to be on same variable to ensure an ordering. If
 * data is protected with a mutex, the exclusive nature of the lock means that the result is indistinguishable from what
 * it would have been had the lock and unlock been sequentially consistent operations. Similarly, if you use acquire and
 * release orderings on atomic variables to build a simple lock, then from the point of view of the code that uses the
 * lock, the behavior will appear sequentially consistent, even though the internal operations are not.
 *
 * If you don't need the stringency of sequentially consistent ordering for your atomic operations, the pair-wise synchronization
 * of acquire-release ordering has the potential for a much lower synchronization cost than the global ordering required
 * for sequentially consistent operations. The trade-off here is the mental cost required to ensure that the ordering
 * works correctly and that the non-intuitive behavior across threads isn't problematic.
 */
int main() {
    std::thread t1(thread_1);
    std::thread t2(thread_2);
    std::thread t3(thread_3);

    t1.join();
    t2.join();
    t3.join();
}