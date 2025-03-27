//
// Created by coolk on 24-03-2025.
//

#ifndef SPINLOCK_H
#define SPINLOCK_H
#include <atomic>

/**
 * std::atomic_flag is the simplest standard atomic type, which represents a Boolean flag. Objects of this type can be
 * in one of two states: set or clear. Objects of type std::atomic_flag must be initialized with ATOMIC_FLAG_INIT. This
 * initializes the flag to a clear state. There's no choice in this matter: the flag always starts clear. It's the only
 * atomic type to require such special treatment for initialization, but it's also the only type guaranteed to be lock-free.
 * Once you have your flag object initialized, there are only three things you can do with it: destroy it, clear it, or
 * set it and query the previous value.
 *
 * You can't copy-construct another std::atomic object from the first, and you can't assign one std::atomic object to
 * another. All operations on an atomic type are defined as atomic, and assignment and copy-construction involve two
 * objects. A single operation on two distinct objects can't be atomic. In the case of copy-construction or copy-assignment,
 * the value must first be read from one object and then written to the other. These are two separate operations on two
 * separate objects, and the combination can't be atomic. Therefore, these operations aren't permitted.
 *
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
