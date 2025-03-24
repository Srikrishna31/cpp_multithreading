//
// Created by coolk on 24-03-2025.
//

#ifndef SPINLOCK_H
#define SPINLOCK_H
#include <atomic>

/**
 * Each of the operations on the atomic types has an optional memory-ordering argument that can be used to specify the
 * required memory-ordering semantics:
 *      * Store operations, which can have memory_order_relaxed, memory_order_release or memory_order_seq_cst ordering.
 *      * Load operations, which can have memory_order_relaxed, memory_order_consume, memory_order_acquire, or
 *      memory_order_seq_cst ordering.
 *      * Read-modify-write operations, which can have memory_order_relaxed, memory_order_consume, memory_order_acquire,
 *      memory_order_release, memory_order_acq_rel, or memory_order_seq_cst ordering.
 */
class spinlock {
    public:
    explicit spinlock();
    auto lock() -> void;
    auto unlock() -> void;

    private:

    spinlock(const spinlock &) = delete;
    spinlock(spinlock &&) = delete;
    spinlock &operator=(const spinlock &) = delete;
    spinlock &operator=(spinlock &&) = delete;

    std::atomic_flag flag_;
};



#endif //SPINLOCK_H
