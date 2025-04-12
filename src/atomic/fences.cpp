//
// Created by coolk on 12-04-2025.
//

#include <atomic>
#include <thread>
#include <cassert>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y()
{
    x.store(true, std::memory_order_relaxed);   // <--- (1)
    std::atomic_thread_fence(std::memory_order_release);  // <--- (2)
    y.store(true, std::memory_order_relaxed);      // <--- (3)
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed)) {}   // <--- (4)
    std::atomic_thread_fence(std::memory_order_acquire);    // <--- (5)
    if (x.load(std::memory_order_relaxed)) {
        ++z;
    }
}

auto a = false;         // a is now a plain nonatomic variable.
std::atomic<bool> b;
std::atomic<int> c;

void write_a_then_b() {
    a = true;       // <--- (6)  Store to a before the fence
    std::atomic_thread_fence(std::memory_order_release);
    b.store(true, std::memory_order_relaxed);   // <--- (7) store to b after the fence
}

void read_a_then_b() {
    while (!b.load(std::memory_order_relaxed)) {}   // <--- (8) wait until you see the write from (7)
    std::atomic_thread_fence(std::memory_order_acquire);
    if (a)          // <--- (9) This will read the value written by #6
        ++ c;
}

/**
 *
 * Fences
 * Fences are operations that enforce memory-ordering constraints without modifying any data and are typically combined
 * with atomic operations that use the memory_order_relaxed ordering constraints. Fences are global operations and affect
 * the ordering of other atomic operations in the thread that executed the fence. Fences are also commonly called
 * memory barriers, and they get their name because they put a line in the code that certain operations can't cross.
 * Relaxed operations on separate variables can usually be freely reordered by the compiler or the hardware. Fences
 * restrict this freedom and introduce happens-before and synchronizes-with relationships that weren't present before.
 *
 * The release fence (2) synchronizes-with the acquire fence (5), because the load from y at (4) reads the value stored
 * at (3). This means that the store to x at (1) happens-before the load from x at (6), so the value read must be true
 * and the assert at (7) won't fire. This is in contrast to the original case without the fences where the store to and
 * load from x weren't ordered, and so the assert could fire. Note that both fences are necessary: you need a release in
 * one thread and an acquire in another to get a synchronizes-with relationship.
 *
 * This is the general idea with fences: if an acquire operation sees the result of a store that takes place after a
 * release operation synchronizes-with that acquire operation; and if a load that takes place before an acquire fence
 * sees the result of a release operation, the release operation synchronizes-with the acquire fence. Although the fence
 * synchronization depends on the values read or written by operations before or after the fence, it's important to note
 * that the synchronization point is the fence itself.
 *
 * However, the real benefit to using atomic operations to enforce ordering is that they can enforce an ordering on
 * nonatomic operations and thus avoid the undefined behavior of a data race.
 *
 * The fences still provide an enforced ordering of the store to a (6) and the store to b (7) and the load from b (8) and
 * the load from a (9), and there's still a happens-before relationship between the store to a and the load from a, so the
 * assert (10) still won't fire. The store to (7) and load from (8) b still have to be atomic; otherwise there would be
 * a data race on b, but the fences enforce an ordering on the operations on a, once the reading thread has seen the stored
 * value of b. This enforced ordering means that there's no data race on  a, even though it's modified by one thread and
 * read by another.
 *
 * Ordering of nonatomic operations through the use of atomic operations is where the sequenced-before part of happens-before
 * becomes so important. If a nonatomic operation is sequenced before an atomic operation, and that atomic operation
 * happens-before an operation in another thread, the nonatomic operation also happens-before that operation in the other
 * thread.
 *
 * The lock() operation is a loop on flag.test_and_set() using std::memory_order_acquire ordering, and the unlock() is a
 * call to flag.clear() with std::memory_order_release ordering. When the first thread calls lock(), the flag is initially
 * clear, so the first call to test_and_set() will set the flag and return false, indicating that this thread now has the
 * lock, and terminating the loop. The thread is then free to modify any data protected by the mutex. Any other thread
 * that calls lock() at this time will find the flag already set and will be blocked in the test_and_set() loop.
 * When the thread with the lock has finished modifying the protected data, it calls unlock(), which calls flag.clear()
 * with std::memory_order_release semantics. This then synchronizes-with a subsequent call to flat.test_and_set() from
 * an invocation of lock() on another thread, because this call has std::memory_order_acquire semantics. Because the
 * modification of the protected data is necessarily sequenced before the unlock() call, this modification happens-before
 * the unlock() and thus happens-before the subsequent lock() call from the second thread (because of the synchronizes-with
 * relationship between the unlock() and the lock()) and happens-before any accesses to that data from this second thread
 * once it has acquired the lock.
 *
 * Although other mutex implementations will have different internal operations, the basic principle is the same: lock()
 * is an acquire operation on an internal memory location, and unlock() is a release operation on that same memory
 * location.
 */
int main() {
    x=false;
    y=false;
    z=0;
    std::thread xt (write_x_then_y);
    std::thread yt(read_y_then_x);
    xt.join();
    yt.join();
    assert(z.load() !=0);

    a = false;
    b = false;
    c = 0;
    std::thread at(write_a_then_b);
    std::thread bt(read_a_then_b);
    at.join();
    bt.join();
    assert(c.load() != 0);
}