//
// Created by coolk on 14-05-2025.
//

#include "interface_machine.h"

#include <iostream>

#include "messages.h"
#include "include/message_queue/dispatcher.h"

void interface_machine::done() {
    get_sender().send(messaging::close_queue());
}

void interface_machine::run() {
    try {
        for (; ;) {
            incoming.wait()
            .handle<issue_money>(
                [&](const issue_money& issue) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Issuing " << issue.amount << std::endl;
                })
            .handle<display_insufficient_funds>(
                [&](const display_insufficient_funds& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Insufficient funds" << std::endl;
                })
            .handle<display_enter_pin>(
                [&](const display_enter_pin& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Please enter your PIN (0-9)" << std::endl;
                })
            .handle<display_enter_card>(
                [&](const display_enter_card& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Please enter your CARD (I)" << std::endl;
                })
            .handle<display_balance>(
                [&](const display_balance& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "The balance of your account is " << issued.amount << std::endl;
                })
            .handle<display_withdrawal_options>(
                [&](const display_withdrawal_options& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Withdraw 50? (w)" << std::endl;
                    std::cout << "Display Balance? (b)" << std::endl;
                    std::cout << "Cancel? (c)" << std::endl;
                })
            .handle<display_withdrawal_cancelled>(
                [&](const display_withdrawal_cancelled& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Withdrawal Cancelled" << std::endl;
                })
            .handle<display_pin_incorrect_message>(
                [&](const display_pin_incorrect_message& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "PIN incorrect" << std::endl;
                })
            .handle<eject_card>(
                [&](const eject_card& issued) {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout << "Ejecting card" << std::endl;
                });
        }
    } catch (messaging::close_queue&) {}
}

messaging::sender interface_machine::get_sender() {
    return incoming;
}
