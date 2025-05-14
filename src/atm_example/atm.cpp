#include "atm.h"
#include "messages.h"
#include "include/message_queue/dispatcher.h"

void atm::process_withdrawal() {
    incoming.wait()
        .handle<withdraw_ok>(
            [&](withdraw_ok const & msg) {
                interface_hardware.send(issue_money(withdrawal_amount));
                bank.send(
                    withdrawal_processed(account, withdrawal_amount));
                state = &atm::done_processing;
            });
}

void atm::process_balance() {
    incoming.wait()
    .handle<balance>(
        [&](balance const & msg) {
            interface_hardware.send(display_balance(msg.amount));
            state=&atm::wait_for_action;
        })
    .handle<cancel_pressed>(
        [&](cancel_pressed const & msg) {
            state=&atm::done_processing;
        });
}

void atm::wait_for_action() {
    interface_hardware.send(display_withdrawal_options());
    incoming.wait()
    .handle<withdraw_pressed>(
        [&](withdraw_pressed const & msg) {
            withdrawal_amount=msg.amount;
            bank.send(account, msg.amount, incoming);
            state=&atm::process_withdrawal;
        })
    .handle<balance_pressed>(
        [&](balance_pressed const & msg) {
            bank.send(get_balance(account, incoming));
            state=&atm::process_balance;
        })
    .handle<cancel_pressed>(
        [&](cancel_pressed const & msg) {
            state=&atm::done_processing;
        });
}

void atm::verifying_pin() {
    incoming.wait()
    .handle<pin_verified>(
        [&](pin_verified const & msg) {
            state=&atm::wait_for_action;
        })
    .handle<pin_incorrect>(
        [&](pin_incorrect  const & msg) {
            interface_hardware.send(
                display_pin_incorrect_message());
            state=&atm::done_processing;
        })
    .handle<cancel_pressed>(
        [&](cancel_pressed const & msg) {
            state=&atm::done_processing;
        });
}

void atm::getting_pin() {
    incoming.wait()
    .handle<digit_pressed>(
        [&](digit_pressed const & msg) {
            unsigned const pin_length=4;
            pin += msg.digit;
            if (pin.length()==pin_length) {
                bank.send(verify_pin(account, pin, incoming));
                state=&atm::verifying_pin;
            }
        })
    .handle<clear_last_pressed>(
        [&](clear_last_pressed const & msg) {
            if (!pin.empty()) {
                pin.pop_back();
            }
        }
        )
    .handle<cancel_pressed>(
        [&](cancel_pressed const & msg) {
            state=&atm::done_processing;
        });
}

void atm::waiting_for_card() {
    interface_hardware.send(display_enter_card());
    incoming.wait()
    .handle<card_inserted>(
        [&](card_inserted const & msg) {
            account=msg.account;
            pin="";
            interface_hardware.send(display_enter_card());
            state=&atm::getting_pin;
        });
}

void atm::done_processing() {
    interface_hardware.send(eject_card());
    state=&atm::waiting_for_card;
}

void atm::done() {
    get_sender().send(messaging::close_queue());
}

void atm::run() {
    state=&atm::waiting_for_card;
    try {
        for (; ;) {
            (this->*state)();
        }
    }
    catch (messaging::close_queue const & ) {}
}

messaging::sender atm::get_sender() {
    return incoming;
}
