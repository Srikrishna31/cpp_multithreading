//
// Created by coolk on 27-03-2025.
//
#include <vector>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

std::vector<int> data;
std::atomic<bool> data_ready{false};

void reader_thread() {
    while (!data_ready.load()) {
        // std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   // <---- (1)
    }
    std::cout << "The answer=" << data[0] << std::endl;    // <----- (2)
}

void writer_thread() {
    data.push_back(42);    // <---- (3)
    data_ready = true;      //  <---- (4)
}

void foo (int a, int b) {
    std::cout << a << ", " << b << std::endl;
}

int get_num() {
    static int i = 0;
    return ++i;
}

/**
 *
 * Synchronizing operations and enforced ordering
 *
 * The required enforced ordering comes from the operations on the std::atomic<bool> variable data_ready; they provide
 * the necessary ordering by virtue of the memory model relations happens-before and synchronizes-with. The write of the
 * data (3) happens-before the write to the data_ready flag (4), and the read of the flag (1) happens-before the read of
 * the data (2). When the value read from data_ready (1) is true the write-synchronizes with that read, creating a happens-before
 * relationship.
 * Because happens-before is transitive, the write to the data (3) happens-before the write to the flag (4), which
 * happens-before the read of the true value from the flag (1), which happens-before the read of the data (2), and you
 * have an enforced ordering; the write of the data happens-before the read of the data and everything is OK.
 *
 * The synchronizes-with relationship
 *
 * The basic idea is this: a suitably tagged atomic write operation W on a variable x synchronizes-with a suitably tagged
 * atomic read operation on X that reads the value stored by either that write (W), or a subsequent atomic write operation
 * on x by the same thread that performed the initial write W, or a sequence of atomic read-modify-write operations on
 * x (such as fetch_add() or compare_exchange_weak()) by any thread, where the value read by the first thread in the
 * sequence is the value written by W.
 *
 * If thread A stores a value and thread B reads that value, there's a synchronizes-with relationship between the store
 * in thread A and the load in thread B.
 *
 *
 * The happens-before relationship
 *
 * The happens-before relationship is the basic building block of operations ordering in a program; it specifies which
 * operations see the effects of which other operations. For a single thread, it's largely straightforward: if one operation
 * is sequenced before another, then it also happens-before it. This means that if one operation (A) occurs in a statement
 * prior to another (B) in the source code, then A happens-before B. If the operations occur in the same statement, in
 * general there's no happens-before relationship between them, because they're unordered. This is just another way of
 * saying that the ordering is unspecified.
 *
 * There are circumstances where operations within a single statement are sequenced such as where the built-in comma
 * operator is used or where the result of one expression is used as an argument to another expression. But in general,
 * operations within a single statement are nonsequenced, and there's no sequenced-before (and thus no happens-before)
 * relationship between them. Ofcourse, all operations in a statement happen before all the operations in the next
 * statement.
 *
 * The new part is the interaction between threads: if operation A on one thread inter-thread happens-before operation B
 * on another thread, then A happens-before B. If operation A in one thread synchronizes-with operation B in another
 * thread, then A inter-thread happens-before B. It's also a transitive relation: if A inter-thread happens-before B and
 * B inter-thread happens-before C, then A inter-thread happens-before C.
 *
 * Inter-thread happens-before also combines with the sequenced-before relation: if operation A is sequenced before
 * operation B, and operation B inter-thread happens-before operation C, then A inter-thread happens-before C. Similarly,
 * if A synchronizes-with B and B is sequenced before C, then A inter-thread happens-before C. These two together mean
 * that if you make a series of changes to data in a single thread, you need only one synchronizes-with relationship for
 * the data to be visible to subsequent operations on the thread that executed C.
 */
int main() {
    std::thread t(reader_thread);
    std::thread t_writer(writer_thread);
    t.join();
    t_writer.join();

    // Order of evaluation of arguments to a function call is unspecified.
    // Calls to get_num are unordered
    foo(get_num(), get_num());
}
