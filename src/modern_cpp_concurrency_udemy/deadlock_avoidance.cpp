//
// Created by coolk on 26-05-2025.
//

#include <iostream>
#include <mutex>

std::mutex mut1;
std::mutex mut2;
using namespace std::literals;

auto funcA() {
    std::cout << "Thread A trying to lock mutexes 1 and 2..." << std::endl;
    std::scoped_lock scoped_lck(mut1, mut2);
    std::cout << "Thread A has locked mutexes 1 and 2..." << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes 1 and 2..." << std::endl;
}

auto funcB() {
    std::cout << "Thread B trying to lock mutexes 1 and 2..." << std::endl;
    std::scoped_lock scoped_lck(mut2, mut1);
    std::cout << "Thread B has locked mutexes 1 and 2..." << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releasing mutexes 1 and 2..." << std::endl;
}

auto funcA_withAdoptLock() {
    std::cout << "Thread A trying to lock mutexes 1 and 2..." << std::endl;
    std::lock(mut1, mut2);      // Lock both mutexes
    std::cout << "Thread A has locked mutexes 1 and 2..." << std::endl;

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lk1{mut1, std::adopt_lock};   // Associate each mutex with a unique_lock
    std::unique_lock<std::mutex> uniq_lk2{mut2, std::adopt_lock};
    std::cout << "Thread A has adopted the locks " << std::endl;

    std::this_thread::sleep_for(50ms);

    std::cout << "Thread A releasing mutexes 1 and 2..." << std::endl;
}

auto funcB_withAdoptLock() {
    std::cout << "Thread B trying to lock mutexes 1 and 2..." << std::endl;
    std::lock(mut2, mut1);
    std::cout << "Thread B has locked mutexes 1 and 2..." << std::endl;

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lk1{mut1, std::adopt_lock};
    std::unique_lock<std::mutex> uniq_lk2{mut2, std::adopt_lock};
    std::cout << "Thread B has adopted the locks " << std::endl;

    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releasing mutexes 1 and 2..." << std::endl;
}

auto funcA_withTryLock() {
    std::unique_lock<std::mutex> uniq_lk1{mut1, std::defer_lock};
    std::unique_lock<std::mutex> uniq_lk2{mut2, std::defer_lock};

    std::cout << "Thread A trying to lock mutexes 1 and 2..." << std::endl;

    auto idx = std::try_lock(uniq_lk1, uniq_lk2);
    if (idx != -1) {
        std::cout << "try_lock failed on mutex with index " << idx << std::endl;
    } else {
        std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;
        std::this_thread::sleep_for(50ms);
        std::cout << "Thread A releasing mutexes 1 and 2..." << std::endl;
    }
}

auto funcB_withTryLock() {
    std::unique_lock<std::mutex> uniq_lk1{mut1, std::defer_lock};
    std::unique_lock<std::mutex> uniq_lk2{mut2, std::defer_lock};

    std::cout << "Thread B trying to lock mutexes 1 and 2..." << std::endl;

    auto idx = std::try_lock(uniq_lk2, uniq_lk1);
    if (idx != -1) {
        std::cout << "try_lock failed on mutex with index " << idx << std::endl;
    } else {
        std::cout << "Thread B has locked mutexes 1 and 2" << std::endl;
        std::this_thread::sleep_for(50ms);
        std::cout << "Thread B releasing mutexes 1 and 2..." << std::endl;
    }
}

/**
 *
 *  Deadlock avoidance
 *      * Make the threads acquire locks in the same order
 *          . Relies on the programmer
 *      * Lock multiple mutexes in a single operation
 *          . Thread A locks mut1 and mut2
 *          . Thread B cannot lock mut2 or mut1 during this operation
 *          . A much better solution
 *      * C++17 has std::scoped_lock
 *          . Very similar to std::lock_guard, except that it can lock more than one mutex at the same time
 *              -> std::scoped_lock scope_lck(mut1, mut2, ...);
 *          . The mutexes are locked in the order given in the constructor call
 *              -> In the destructor, the mutexes are unlocked in the reverse order.
 *          . This avoids the possibility of deadlock with multiple mutexes.
 *      * std::scoped_lock Caveat
 *          . scoped_lock can be used with a single mutex
 *          . It is easy to accidentally omit the argument
 *          . This will compile and run, but not actually perform any locking, and may cause an unexpected data race.
 *      * std::try_lock()
 *          . Also locks multiple mutexes in a single operation
 *          . Returns immediately if it cannot obtain all the locks.
 *              # On failure it returns the index of the object that failed to lock(0 for the first one and so on).
 *              # On success it returns -1
 *      * Hierarchical Mutex
 *          . Sometimes it is not feasible to acquire multiple locks simultaneously
 *          . A common technique is to impose ordering
 *          . A thread cannot lock a mutex unless it has already locked a mutex with a lower status
 *              # ID number
 *              # Alphabetical name
 *      * Deadlock Avoidance Guidelines
 *          . Avoid waiting for a thread while holding a lock
 *          . Try to avoid waiting for other threads
 *          . Try to avoid nested locks
 *              # If your thread already holds a lock, do not acquire another one
 *              # If you need multiple locks, acquire them in a single operation
 *          . Avoid calling functions within a critical section
 *              # unless you are certain the function doesnot try to lock.
 */
auto main() -> int {
    std::thread thrA{funcA};
    std::thread thrB{funcB};

    thrA.join();
    thrB.join();

    std::cout << "-----------------------------" << std::endl;

    std::thread thrC{funcA_withAdoptLock};
    std::thread thrD{funcB_withAdoptLock};

    thrC.join();
    thrD.join();

    std::cout << "-----------------------------" << std::endl;

    std::thread thrE{funcA_withTryLock};
    std::thread thrF{funcB_withTryLock};

    thrE.join();
    thrF.join();

    return 0;
}