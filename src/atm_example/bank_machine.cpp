//
// Created by coolk on 14-05-2025.
//

#include "bank_machine.h"

#include "messages.h"
#include "include/message_queue/dispatcher.h"

bank_machine::bank_machine()
    :balance(199)
{

}

void bank_machine::done() {
    get_sender().send(messaging::close_queue());
}

void bank_machine::run() {
    try {
        for (; ;) {
            incoming.wait()
            .handle<verify_pin>(
                [&](verify_pin const& msg) {
                    if (msg.pin == "1937") {
                        msg.atm_queue.send(pin_verified());
                    } else {
                        msg.atm_queue.send(pin_incorrect());
                    }
                })
            .handle<withdraw>(
                [&](withdraw const& msg) {
                    if (balance >= msg.amount) {
                        msg.atm_queue.send(withdraw_ok());
                        balance -= msg.amount;
                    }
                    else {
                        msg.atm_queue.send(withdraw_denied());
                    }
                })
            .handle<get_balance>(
                [&](get_balance const& msg) {
                    msg.atm_queue.send(::balance(balance));
                })
            .handle<withdrawal_processed>(
                [&](withdrawal_processed const& msg) {})
            .handle<cancel_withdrawal>(
                [&](cancel_withdrawal const& msg) {});
        }
    } catch (messaging::close_queue const& e) {}
}

messaging::sender bank_machine::get_sender() {
    return incoming;
}
