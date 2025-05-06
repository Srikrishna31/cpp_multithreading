#pragma once
#include <memory>

namespace messaging {
    /**
     * The TemplateDispatcher<> class template is modeled on the dispatcher class and is almost identical. Since you don't
     * throw exceptions if you handle the message, you now need to check whether you did handle the message in your
     * message loop. Your message processing stops when you've successfully handled a message, so that you can wait for
     * a different set of messages next time. If you do get a match for the specified message type, the supplied function
     * is called rather than throwing an exception (although the handler function may throw an exception itself). If you
     * don't get a match, you chain to the previous dispatcher. In the first instance, this will be a dispatcher, but if
     * you chain calls to handle() to allow multiple types of messages to be handled this may be a prior instantiation
     * of TemplateDispatcher<>, which will in turn chain to the previous handler if the message doesn't match. Because
     * any of the handlers might throw an exception (including the dispatcher's default handler for close_queue messages),
     * the destructor must once again be declared noexcept(false).
     * This simple framework allows you to push any type of message on the queue and then selectively match against
     * messages you can handle on the receiving end. It also allows you to pass around a reference to the queue for
     * pushing messages on, while keeping the receiving end private.
     * @tparam PreviousDispatcher
     * @tparam Msg
     * @tparam Func
     */
    template<typename PreviousDispatcher, typename Msg, typename Func>
    class TemplateDispatcher {
        queue* q{};
        PreviousDispatcher* prev;
        Func f;
        bool chained;

        TemplateDispatcher(const TemplateDispatcher& rhs) = delete;
        TemplateDispatcher& operator=(const TemplateDispatcher& rhs) = delete;

        template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
        friend class TemplateDispatcher;        // TemplateDispatcher instantiations are friends of each other.

        void wait_and_dispatch() {
            for (; ;) {
                auto msg = q->wait_and_pop();
                if (dispatch(msg))
                    break;          // If we handle the message, break out of the loop
            }
        }
        bool dispatch(std::shared_ptr<message_base> const& msg) {
            if (wrapped_message<Msg>* wrapper = dynamic_cast<wrapped_message<Msg>*>(msg.get())) {
                // Check the message type, and call the function.
                f(wrapper->contents);
                return true;
            } else {
                return prev->dispatch(msg);     // Chain to the previous dispatcher.
            }
        }
    public:
        TemplateDispatcher(TemplateDispatcher&& other):
            q(other.q), prev(other.prev), f(std::move(other.f)), chained(other.chained) {
            other.chained = true;
        }
        TemplateDispatcher(queue* q_, PreviousDispatcher* prev_, Func&& f_):
            q(q_), prev(prev_), f(std::move(f_)), chained(false) {
            prev->chained = true;
        }
        template<typename OtherMsg, typename OtherFunc>
        TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
        handle(OtherFunc&& of) {    // Additional handlers can be chained.
            return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
                q, this, std::forward<OtherFunc>(of));
        }
        ~TemplateDispatcher() noexcept(false) {
            if (!chained) {
                wait_and_dispatch();
            }
        }
    };
}
