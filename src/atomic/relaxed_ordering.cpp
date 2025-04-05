//
// Created by coolk on 05-04-2025.
//

/**
 * Relaxed Ordering
 * Operations on atomic types performed with relaxed ordering don't participate in synchronizes-with relationships.
 * Operations on the same variable within a single thread still obey happens-before relationships, but there's almost no
 * requirement on ordering relative to other threads. The only requirement is that accesses to a single atomic variable
 * from the same thread can't be reordered; once a given thread has seen a particular value of an atomic variable, a
 * subsequent read by that thread can't retrieve an earlier value of the variable. Without any additional synchronization
 * the modification order of each variable is the only thing shared between threads that are using memory_order_relaxed.
 *
 */

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);   // <--- (1)
    y.store(true, std::memory_order_relaxed);   // <--- (2)
}

void read_x_then_y() {
    while (!y.load(std::memory_order_relaxed)) {}    // <--- (3)
    if (x.load(std::memory_order_relaxed))           // <--- (4)
        ++z;
}

/**
 * This time the assert (5) can fire, because the load of x (4) can read false, even though the load of y (3) reads true
 * and the store of x (1) happens-before the store of y (2). x and y are different variables, so there are no ordering
 * guarantees relating to the visibility of values arising from operations on each.
 *
 * Relaxed operations on different variables can be freely reordered provided they obey any happens-before relationships
 * they're bound by (for example, within the same thread). They don't introduce synchronizes-with relationships. Even though
 * there's a happens-before relationship between the stores and between the loads, there isn't one between either store
 * and either load, and so the loads can see the stores out of order.
 *
 *
 */
int main() {
    x = false;
    y = false;
    z = 0;
    std::thread t1(write_x_then_y);
    std::thread t2(read_x_then_y);
    t1.join();
    t2.join();
    assert(z.load() != 0);       // <--- (5)
}