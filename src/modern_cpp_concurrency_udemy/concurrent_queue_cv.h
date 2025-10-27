#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <chrono>

using namespace std::literals;

/**
 *    * This is a simple concurrent queue.
 *    * It employs "coarse-grained" locking
 *         - Only one thread can access the queue at any one time.
 *         - In effect, the program becomes single-threaded.
 *    * Adding the condition variable improves this slightly
 *         - If the queue is empty and a thread is trying to pop()
 *         - Other threads can run, until the queue is no longer empty.
 *    * A lock free solution is possible and more efficient, but it is more complex.
 */
template<typename T>
class concurrent_queue_cv {
    mutable std::mutex mut;
    std::queue<T> que;
    std::condition_variable cond_var;
    int max{50};
public:
    concurrent_queue_cv() = default;
    concurrent_queue_cv(int max):max(max) {};

    concurrent_queue_cv(const concurrent_queue_cv&) = delete;
    concurrent_queue_cv& operator=(const concurrent_queue_cv&) = delete;
    concurrent_queue_cv(concurrent_queue_cv&&) = delete;
    concurrent_queue_cv& operator=(concurrent_queue_cv&&) = delete;

    auto push(T value) -> void {
        std::unique_lock<std::mutex> uniq_lck(mut);

        while(que.size() > max) {
            uniq_lck.unlock();
            // TODO: Use another condition variable instead of sleep
            std::this_thread::sleep_for(50ms);
            uniq_lck.lock();
        }

        que.push(value);
        cond_var.notify_one();
    }

    auto pop(T& value) -> void {
        std::unique_lock<std::mutex> uniq_lck(mut);

        cond_var.wait(uniq_lck, [this]{return !que.empty();});

        value = std::move(que.front());
        que.pop();
    }

    auto empty() const -> bool {
        std::lock_guard<std::mutex> lck{mut};
        return que.empty();
    }
};
