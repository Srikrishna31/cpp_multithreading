//
// Created by coolk on 14-05-2025.
//

#include <iostream>

#include "atm.h"
#include "bank_machine.h"
#include "interface_machine.h"
#include "messages.h"

int main() {
    bank_machine bank;
    interface_machine interface_hardware;

    atm machine(bank.get_sender(), interface_hardware.get_sender());

    std::thread bank_thread(&bank_machine::run, &bank);
    std::thread if_thread(&interface_machine::run, &interface_hardware);
    std::thread atm_thread(&atm::run, &machine);

    messaging::sender atmqueue(machine.get_sender());

    bool quit_pressed = false;

    while (!quit_pressed) {
        char c=getchar();
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                atmqueue.send(digit_pressed(c));
                break;
            case 'b':
                atmqueue.send(balance_pressed());
                break;
            case'w':
                atmqueue.send(withdraw_pressed(50));
                break;
            case'c':
                atmqueue.send(cancel_pressed());
                break;
            case 'q':
                quit_pressed=true;
                break;
            case 'i':
                atmqueue.send(card_inserted("acc1234"));
                break;
            default:
                std::cout << "Unknown option provided. Please choose from [b, w, c, q, i]";
                break;
        }
    }

    bank.done();
    machine.done();
    interface_hardware.done();
    atm_thread.join();
    bank_thread.join();
    if_thread.join();

    return 0;
}
