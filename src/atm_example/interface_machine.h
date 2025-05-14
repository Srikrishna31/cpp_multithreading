//
// Created by coolk on 14-05-2025.
//

#ifndef INTERFACE_MACHINE_H
#define INTERFACE_MACHINE_H
#include "include/message_queue/receiver.h"


class interface_machine {
    messaging::receiver incoming;
    std::mutex iom;

public:
    void done();

    void run();

    messaging::sender get_sender();
};



#endif //INTERFACE_MACHINE_H
