//
// Created by coolk on 24-03-2025.
//

#include "atomic/spinlock.h"

spinlock::spinlock()
    : flag_(ATOMIC_FLAG_INIT)
{}

auto spinlock::lock() -> void {
    while (flag_.test_and_set(std::memory_order_acquire));
}

auto spinlock::unlock() -> void {
    flag_.clear(std::memory_order_release);
}