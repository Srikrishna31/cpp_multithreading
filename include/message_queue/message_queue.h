#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace messaging {
    struct message_base {   // Base class of queue entries.
        virtual ~message_base() = default;
    };

    template<typename Msg>
    struct wrapped_message_base : message_base {
        Msg contents;

        explicit wrapped_message_base(Msg contents_) : contents(contents_) {}
    };   // Each message type has a specialization

    class queue {       // Message Queue
        std::mutex m;
        std::condition_variable cv;
        std::queue<std::shared_ptr<message_base>> q; // Actual queue stores pointers to message base.

    public:
        template<typename Msg>
        void push (Msg msg) {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::make_shared<wrapped_message_base<Msg>>(std::move(msg)));    // Wrap posted message and store pointer.
            cv.notify_all();
        }
        std::shared_ptr<message_base> wait_and_pop() {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&] { return !q.empty(); });
            auto res = q.front();
            q.pop();
            return res;
        }
    };
}