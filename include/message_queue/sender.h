#pragma once
#include "message_queue.h"

namespace messaging {
    class sender {
        queue* q;   // sender is wrapper around queue pointer.
        public:
        sender()    // Default constructed sender has no queue.
            :q{nullptr}
        {}

        explicit sender(queue* q_)  // Allow construction from pointer to queue.
            : q{q_}
        {}

        template<typename Message>
        void send (Message msg) {
            if (q) {
                q->push(msg);   // Sending pushes message on the queue.
            }
        }
    };
}
