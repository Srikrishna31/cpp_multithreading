//
// Created by coolk on 24-03-2025.
//

#include "spinlock.h"

/**
 * Each of the operations on the atomic types has an optional memory-ordering argument that can be used to specify the
 * required memory-ordering semantics:
 *      * Store operations, which can have memory_order_relaxed, memory_order_release or memory_order_seq_cst ordering.
 *      * Load operations, which can have memory_order_relaxed, memory_order_consume, memory_order_acquire, or
 *      memory_order_seq_cst ordering.
 *      * Read-modify-write operations, which can have memory_order_relaxed, memory_order_consume, memory_order_acquire,
 *      memory_order_release, memory_order_acq_rel, or memory_order_seq_cst ordering.
 *
 *
 *   Operations on std::atomic<bool>
 *
 * Another common pattern with the atomic types: the assignment operators they support return values (of the corresponding
 * nonatomic type) rather than references. If a reference to the atomic variable was returned, any code that depended on
 * the result of the assignment would then have to explicitly load the value, potentially getting the result of a modification
 * by another thread. By returning the result of the assignment as a nonatomic value, you can avoid this additional load,
 * and you know that the value obtained is the actual value stored.
 *
 * It supports three operations: load (a load operation), store (a store operation) and exchange (read-modify-write operation).
 *
 * In addition to exchange(), it also introduces an operation to store a new value if the current value is equal to an
 * expected value. This new operation is called compare/exchange, and it comes in the form of the `compare_exchange_weak()`
 * and `compare_exchange_strong()` member functions.
 *
 * The compare/exchange operation is the cornerstone of programming with atomic types; it compares the value of the atomic
 * variable with a supplied expected value and stores the supplied desired value if they're equal. If the values aren't
 * equal, the expected value is updated with the actual value of the atomic variable. The return type of the compare/exchange
 * functions is a bool, which is true if the store was performed and false otherwise.
 *
 * For compare_exchange_weak(), the store might not be successful even if the original value was equal to the expected
 * value, in which case the value of the variable is unchanged, and the return value of compare_exchange_weak() is false.
 * This is most likely to happen on machines that lack a single compare-and-exchange instruction, if the processor can't
 * guarantee that the operation has been done atomically--possibly because the thread performing the operation was switched
 * out in the middle of the necessary sequence of instructions and another thread scheduled in its place by the OS, where
 * there are more threads than processors. This is called a spurious failure, because the reason for the failure is a
 * function of timing rather than the values of the variables.
 *
 * Because compare_exchange_weak() can fail spuriously, it must typically be used in a loop:
 *  bool expected = false;
 *  extern atomic<bool> b; // set somewhere else
 *  while (!b.compare_exchange_weak(expected, true) && !expected);
 *
 * On the other hand, compare_exchange_strong() is guaranteed to return false only if the actual value wasn't equal to
 * the expected value. This can eliminate the need for loops.
 *
 * One difference between std::atomic<bool> and std::atomic_flag is that std::atomic<bool> may not be lock-free; the
 * implementation may have to acquire a mutex internally in order to ensure the atomicity of the operations.
 *
 *      The std::atomic<> primary class template
 *
 * The presence of the primary template allows a user to create an atomic variant of a user-defined type, in addition to
 * the standard atomic types. In order to use std::atomic<UDT> for some user-defined type UDT, this type must have a
 * trivial copy-assignment operator. This means that the type must not have any virtual functions or virtual base classes
 * and must use the compiler-generated copy-assignment operator. Not only that, but every base class and non-static data
 * member of a user-defined type must also have a trivial copy-assignment operator. This essentially permits the compiler
 * to use memcpy() or an equivalent operation for assignment operations, because there's no user-written code to run.
 *  Finally, the type must be bitwise equality comparable. This goes alongside the assignment requirements; not only must
 *  you be able to copy an object of type UDT using memcpy(), but you must be able to compare instances for equality using
 *  memcmp(). This guarantee is required in order for compare/exchange operations to work.
 *
 *  The reasoning behind these restrictions goes back to one of the guidelines: don't pass pointers and references to
 *  protected data outside the scope of the lock by passing them as arguments to user-supplied functions. In general, the
 *  compiler isn't going to be able to generate lock-free code for std::atomic<UDT>, so it will have to use an internal
 *  lock for all the operations. If user-supplied copy-assignment or comparison operators were permitted, this would
 *  require passing a reference to the protected data as an argument to a user-supplied function, thus violating the
 *  guideline. These restrictions increase the chance that the compiler will be able to make use of atomic instructions
 *  directly for std::atomic<UDT> (and thus make a particular instantiation lock-free), because it can just treat the
 *  user-defined type as a set of raw bytes.
 *
 *  Note that although you can use std::atomic<float> or std::atomic<double>, because the built-in floating point types
 *  do satisfy the criteria for use with memcpy and memcmp, the behavior may be surprising in the case of compare_exchange_strong.
 *  The operation may fail even though the old stored value was equal in value to the comparand, if the stored value had
 *  a different representation.
 *
 *  When instantiated with a user-defined type T, the interface of std::atomic<T> is limited to the set of operations
 *  available for std::atomic<bool>: load(), store(), exchange(), compare_exchange_weak(), compare_exchange_strong(), and
 *  assignment from and conversion to an instance of type T.
 *
 *  The standard atomic types do more than just avoid the undefined behavior associated with a data race; they allow the
 *  user to enforce an ordering of operations between threads.
 */
spinlock::spinlock()
    : flag_{}
{}

auto spinlock::lock() -> void {
    while (flag_.test_and_set(std::memory_order_acquire));
}

auto spinlock::unlock() -> void {
    flag_.clear(std::memory_order_release);
}