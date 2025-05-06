#pragma once
#include "sender.h"

namespace messaging {
    class receiver {
        queue q;    // A receiver owns the queue.

    public:
        operator sender() { // Allow implicit conversion to a sender that references the queue.
            return sender(&q);
        }

        dispatcher wait() {     // Waiting for a queue creates a dispatcher.
            return dispatcher(&q);
        }
    };
}
