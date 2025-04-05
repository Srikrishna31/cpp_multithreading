//
// Created by coolk on 31-03-2025.
//
#include <atomic>
#include <thread>
#include <cassert>
#include <vector>
#include <algorithm>
/**
 *      Memory ordering for atomic operations
 *  There are six memory ordering options that can be applied to operations on atomic types: memory_order_relaxed,
 *  memory_order_consume, memory_order_acquire, memory_order_release, memory_order_acq_rel, and memory_order_seq_cst.
 *  Although there are six ordering options, they represent three models: sequentially consistent ordering (memory_order_seq_cst),
 *  acquire-release ordering (memory_order_consume, memory_order_acquire, memory_order_release, and memory_order_acq_rel),
 *  and relaxed ordering (memory_order_relaxed).
 *  These distinct memory-ordering models can have varying costs on different CPU architectures. For example, on systems
 *  based on architectures with fine control over the visibility of operations by processors other than the one that made
 *  the change, additional synchronization instructions can be required for sequentially consistent ordering over
 *  acquire-release ordering or relaxed ordering and for acquire-release ordering over relaxed ordering.
 *      The availability of the distinct memory-ordering models allows experts to take advantage of the increased
 *  performance of the more fine-grained ordering relationships where they're advantageous while allowing the use of the
 *  default sequentially consistent ordering (which is considerably easier to reason than the others) for those cases
 *  that are less critical.
 *
 *      Sequentially consistent ordering
 *
 *  It implies that the behavior of the program is consistent with a simple sequential view of the world. If all operations
 *  on instances of atomic types are sequentially consistent, the behavior of a multithreaded program is as if all these
 *  operations were performed in some particular sequence by a single thread. This is by far the easiest memory ordering
 *  to understand, which is why it's the default: all threads must see the same order of operations. This makes it easier
 *  to reason about the behavior of code written with atomic variables. You can write down all the possible sequences of
 *  operations by different threads, eliminate those that are inconsistent, and verify that your code behaves as expected
 *  in the others. It also means that operations can't be reordered; if your code has one operation before the another in
 *  one thread, that ordering must be seen by all the threads.
 *
 *
 */
std::atomic_bool x,y;
std::atomic_int z;

void write_x() {
    x.store(true, std::memory_order_seq_cst);    // <--- (1)
}

void write_y() {
    y.store(true, std::memory_order_seq_cst);    // <--- (2)
}

void read_x_then_y() {
    while (!x.load(std::memory_order_seq_cst)) {}  // <--- (3)
    if (y.load(std::memory_order_seq_cst))
        ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst)) {}  // <--- (4)
    if (x.load(std::memory_order_seq_cst))
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;

    std::thread a{write_x};
    std::thread b{write_y};
    std::thread c{read_x_then_y};
    std::thread d{read_y_then_x};
    a.join();
    b.join();
    c.join();
    d.join();

    assert(z.load() != 0);                        // <--- (5)

    auto v = std::vector<double>{2.0, 1.0, 3.0};
    auto res = std::adjacent_find(v.begin(), v.end(), std::greater_equal<>());
    assert(res == v.end());
}