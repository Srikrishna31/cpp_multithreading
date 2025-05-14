//
// Created by coolk on 14-05-2025.
//

#ifndef BANK_MACHINE_H
#define BANK_MACHINE_H
#include "include/message_queue/receiver.h"

class bank_machine {
    messaging::receiver incoming;
    unsigned balance;

public:
    bank_machine();

    void done();

    void run();

    messaging::sender get_sender();
};



#endif //BANK_MACHINE_H
