//
// Created by coolk on 10-06-2025.
//
#include <algorithm>
#include <vector>
#include <list>
#include <thread>
#include <iostream>

template<typename T>
struct LockFreeQueue {
private:
    std::list<T> lst;

    typename std::list<T>::iterator iHead, iTail;
public:
    LockFreeQueue() {
        //Create a "dummy" element to separate iHead from iTail
        lst.push_back(T());
        iHead = lst.begin();
        iTail = lst.end();
    }

    auto Produce(const T& t) {
        lst.push_back(t);

        iTail = lst.end();

        lst.erase(lst.begin(), iHead);
    }

    auto Consumer(T& t) -> bool {
        auto iFirst = iHead;
        ++iFirst;

        if (iFirst != iTail) {
            iHead = iFirst;

            t = *iHead;
            return true;
        }
        return false;
    }

    auto Print() {
        auto head = iHead;
        ++head;

        std::for_each(head, iTail, [&](const T& t) {
            std::cout << t << ", " << std::endl;
        });

        std::cout << std::endl;
    }
};
/**
 *  Lock-free Programming
 *      * Threads execute critical sections concurrently
 *          # Without data races
 *          # But without using the operating system's locking facilities
 *      * Avoids or reduces some of the drawbacks to using locks
 *          # Race conditions caused by forgetting to lock, or using the wrong mutex.
 *          # Lack of composability
 *          # Risk of deadlock
 *          # High overhead
 *          # Lack of scalability caused by coarse-grained locking
 *          # Code complexity and increased overhead caused by fine-grained locking
 *  Locking vs Lock-free
 *      * Both programming styles are used to manage shared state
 *          # Analogous to managing a traffic intersection
 *      * Locks
 *          # Traffic lights control access
 *          # Stop and wait until able to proceed into critical section
 *      * Lock-free
 *          # Motorway-style intersection
 *          # Traffic from different levels can go over the same section at the same time.
 *          # Traffic from one level can merge with traffic from a different level without stopping.
 *          # If not done carefully, collisions can occur!
 *          # Advantages of Lock-free programming
 *              > No possibility of deadlock of livelock
 *              > If a thread is blocked, other threads can continue to execute.
 *              > Useful if work must be completed within a time limit.
 *              > (e.g. real time systems)
 *          # Drawbacks of lock-free programming
 *              > Very difficult to write code which is correct and efficient
 *              > The extra complexity makes it unsuitable for many applications.
 *                  . e.g. User interface code with separation of concerns
 *                  . Maybe useful in performance-critical code, such as infrastructure
 *              > Should be used only if
 *                  . A data structure in the program is subject to high contention
 *                  . Which causes unacceptable performance.
 *                  . And the lock-free version brings performance up to acceptable levels.
 *          # The strange world of lock-free programming
 *              > Shared data may have different values in different threads
 *              > The value may change between an "if" statement and its body
 *              > Statements may execute in a different order from the source code.
 *          # Transactions
 *              > Transactional model of lock-free programming: "ACID"
 *              > Atomic/All-or-nothing
 *                  . A transaction either completes successfully ("commit")
 *                  . Or it fails and leaves everything as it was ("rollback")
 *              > Consistent
 *                  . The transaction takes the database from one consistent state to another
 *                  . As seen by other users, the database is never in an inconsistent state.
 *              > Isolated
 *                  . Two transactions can never work on the same data simultaneously
 *              > Durable
 *                  . Once a transaction is committed, it cannot be overrwritten
 *                  . ...until the next transaction sees the result of the commit
 *                  . There is no possibility of "losing" an update.
 *              > Transactional Memory
 *                  . Put shared data in transactional memory
 *                  . All operations on shared data will be transactional.
 *                  . However, there is no standard implementation in C++.
 */
auto main() -> int {
    LockFreeQueue<int> lfq;
    std::vector<std::thread> threads;
    int j = 1;

    for (int i = 0; i < 10; ++i) {
            // Run the member functions of LockFreeQueue in threads

        std::thread produce(&LockFreeQueue<int>::Produce, &lfq, std::ref(i));
        threads.push_back(std::move(produce));
        std::thread consume(&LockFreeQueue<int>::Consumer, &lfq, std::ref(j));
        threads.push_back(std::move(consume));
    }

    for (auto& t : threads) {
        t.join();
    }

    lfq.Print();
}