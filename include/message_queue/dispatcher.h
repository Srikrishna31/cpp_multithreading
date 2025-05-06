#pragma once
#include "message_queue.h"

namespace messaging {

    class close_queue   // The message for closing the queue.
    {};

    /**
     * The dispatcher instance that's returned from wait() will be destroyed immediately, because it's a temporary, and
     * as mentioned, the destructor does the work. The destructor calls wait_and_dispatch(), which is a loop that waits
     * for a message and passes it to dispatch(). dispatch() itself is rather simple; it checks whether the message is a
     * close_queue message and throws an exception if it is; otherwise, it returns false to indicate that the message was
     * unhandled.This close_queue exception is why the destructor is marked noexcept(false); without this annotation the
     * default exception specification for destructor would be noexcept(true), indicating that no exceptions can be thrown,
     * and the close_queue exception would thus terminate the program.
     */
    class dispatcher {
        queue* q;
        bool chained;

        dispatcher(dispatcher const&) = delete; // Dispatcher instances cannot be copied.
        dispatcher& operator=(dispatcher const&) = delete;

        template<
            typename Dispatcher,
            typename Msg,
            typename Func>
        friend class TemplateDispatcher;    // Allow TemplateDispatcher instances to access internals.

        [[noreturn]] void wait_and_dispatch() {
            for (; ;) { // Loop waiting for and dispatching messages.
                auto msg = q->wait_and_pop();
                dispatch(msg);
            }
        }

        bool dispatch(std::shared_ptr<message_base> cons& msg) {
            if (dynamic_cast<wrapped_message_base<close_queue>*>(msg.get())) {
                throw close_queue();
            }
            return false;
        }

    public:
        dispatcher(dispatcher&& other) : q(other.q), chained(other.chained) {   // Dispatcher instances can be moved.
            other.chained = true;   // The source mustn't wait for messages
        }
        explicit dispatcher(queue* q_) : q(q_), chained(false) {}

        template<typename Message, typename Func>
        TemplateDispatcher<dispatcher, Message, Func>
        handle(Func&& f) {  // Handle a specific type of message with a TemplateDispatcher.
            return TemplateDispatcher<dispatcher, Message, Func>(q, this,std::forward<Func>(f));
        }

        ~dispatcher() noexcept(false) { // The destructor might throw exceptions
            if (!chained) {
                wait_and_dispatch();
            }
        }
    };
}
