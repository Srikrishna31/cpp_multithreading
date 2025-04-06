//
// Created by coolk on 06-04-2025.
//
#include <atomic>
#include <thread>
#include <cassert>

/**
 *      Acquire Release ordering
 *  Acquire-release ordering is a step up from relaxed ordering: there's still no total order of operations, but it does
 *  produce some synchronization. Under this ordering model, atomic loads are "acquire" operations (memory_order_acquire),
 *  atomic stores are "release" operations (memory_order_release), and atomic read-modify-write operations (such as fetch_add,
 *  or exchange) are either acquire, release or both (memory_order_acq_rel). Synchronization is pairwise, between the
 *  thread that does the release and the thread that does the acquire. A release (write) operation synchronizes-with an
 *  acquire (load) operation that reads the value written. This means that different threads can still see different
 *  orderings, but these orderings are restricted.
 *
 *  Acquire-release doesn't imply a total ordering.
 *
 *
 */
std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    x.store(true, std::memory_order_release);
}

void write_y() {
    y.store(true, std::memory_order_release);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_acquire)) {}
    if (y.load(std::memory_order_acquire)) {   // <--- (1)
        ++z;
    }
}

void read_y_then_x() {
    while (!y.load(std::memory_order_acquire)) {}
    if (x.load(std::memory_order_acquire)) {   // <--- (2)
        ++z;
    }
}

/**
 *
 * In this case the assert (3) can fire (just like in the relaxed-ordering case), because it's possible for both the load
 * of x (2) and the load of y (1) to read false. x and y are written by different threads, so the ordering from the release
 * to the acquire in each case has no effect on the operations in the other threads.
 */
int main() {
    x=false;
    y=false;
    z=0;
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);

    a.join();
    b.join();
    c.join();
    d.join();

    assert(z.load() != 0);    // <--- (3)
}
