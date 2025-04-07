//
// Created by coolk on 07-04-2025.
//
#include <atomic>
#include <thread>
#include <string>
#include <cassert>

struct X {
    int i;
    std::string s;
};
std::atomic<X*> p;
std::atomic<int> a;

void create_x() {
    const auto x = new X;
    x->i = 42;
    x->s = "hello";
    a.store(99, std::memory_order_relaxed);   // <--- (1)
    p.store(x, std::memory_order_release);   // <--- (2)
}

void use_x() {
    X* x;
    while (!(x=p.load(std::memory_order_consume))) {  // <--- (3)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    assert(x->i == 42);                                 // <--- (4)
    assert(x->s == "hello");                            // <--- (5)
    assert(a.load(std::memory_order_relaxed) == 99);    // <--- (6)
}

/**
 *
 *      Data dependency with Acquire-Release ordering and Memory_Order_Consume
 *
 *  There are two new relations that deal with data dependencies: dependency-ordered-before and carries-a-dependency-to.
 *  Just like sequenced-before, carries-a-dependency-to applies strictly within a single thread and essentially models the
 *  data dependency between operations; if the result of an operation A is used as an operand for an operationB, then A
 *  carries-a-dependency-to B. If the result of operation A is a value of a scalar type such as an int, then the relationship
 *  still applies if the result of A is stored in a variable, and that variable is then used as an operand for operation
 *  B. This operation is also transitive, so if A carries-a-dependency-to B, and B carries-a-dependency-to C, then A
 *  carries-a-dependency-to C.
 *
 *  On the other hand, the dependency-ordered-before relationship can apply between threads. It's introduced by using atomic
 *  load operations tagged with memory_order_consume. This is a special case of memory_order_acquire that limits the
 *  synchronized data to direct dependencies; a store operation A tagged with memory_order_release, memory_order_acq_rel,
 *  or memory_order_seq_cst is dependency-ordered-before a load operation B tagged with memory_order_consume if the consume
 *  reads the value stored. This is as opposed to the synchronizes-with relationship you get if the load uses memory_order_acquire.
 *  If this operation B then carries-a-dependency-to some operation C, then A is also dependency-ordered-before C. If A
 *  is dependency-ordered-before B, then A also inter-thread happens-before B.
 *
 *  One important use for this kind of memory ordering is where the atomic operation loads a pointer to some data. By using
 *  memory_order_consume on the load and memory_order_release on the prior store, you ensure that the pointed-to data is
 *  correctly synchronized, without imposing any synchronization requirements on any other nondependent data.
 *
 *  Even though the store to a (1) is sequenced before the store to p (2), and the store to p is tagged memory_order_release,
 *  the load of p (3) is tagged memory_order_consume. This means that the store to p only happens-before those expressions
 *  that are dependent on the value loaded from p. This means that the asserts on the data members of the X structure (4),
 *  (5) are guaranteed not to fire, because the load of p carries a dependency to those expressions through the variable
 *  x. On the other hand, the assert on the value of a (6) may or may not fire; this operations isn't dependent on the value
 *  loaded from p, and so there's no guarantee on the value that's read. This is particularly apparent because it's tagged
 *  with memory_order_relaxed.
 */
int main() {
    std::thread t1(create_x);
    std::thread t2(use_x);

    t1.join();
    t2.join();
}