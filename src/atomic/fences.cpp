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
 */
int main() {
    x=false;
    y=false;
    z=0;
    std::thread a (write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() !=0);
}