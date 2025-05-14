#pragma once
#include <string>
#include <utility>

#include "include/message_queue/sender.h"

struct withdraw {
    std::string account;
    unsigned amount;
    mutable messaging::sender atm_queue;

    withdraw(std::string _account, const unsigned _amount, const messaging::sender _atm_queue):
        account(std::move(_account)), amount(_amount), atm_queue(_atm_queue) {}
};

struct withdraw_ok
{};

struct withdraw_denied
{};

struct cancel_withdrawal {
    std::string account;
    unsigned amount;

    cancel_withdrawal(std::string _account, const unsigned _amount):
        account(std::move(_account)), amount(_amount) {}
};

struct withdrawal_processed {
    std::string account;
    unsigned amount;
    withdrawal_processed(std::string _account, const unsigned _amount):
     account(std::move(_account)), amount(_amount) {}
};

struct card_inserted {
    std::string account;
    explicit card_inserted(std::string _account):
        account(std::move(_account)) {}
};

struct digit_pressed {
    char digit;

    explicit digit_pressed(const char _digit):
        digit(_digit){}
};

struct clear_last_pressed
{};

struct eject_card
{};

struct withdraw_pressed {
    unsigned amount;
    explicit withdraw_pressed(const unsigned _amount):
        amount(_amount){}
};

struct cancel_pressed
{};

struct issue_money {
    unsigned amount;
    issue_money(const unsigned _amount):
        amount(_amount){}
};

struct verify_pin {
    std::string account;
    std::string pin;
    mutable messaging::sender atm_queue;

    verify_pin(std::string _account, std::string _pin, const messaging::sender _atm_queue):
        account(std::move(_account)), pin(std::move(_pin)), atm_queue(_atm_queue) {}
};

struct pin_verified
{};

struct pin_incorrect
{};

struct display_enter_pin
{};

struct display_enter_card
{};

struct display_insufficient_funds
{};

struct display_withdrawal_cancelled
{};

struct display_pin_incorrect_message
{};

struct display_withdrawal_options
{};

struct get_balance {
    std::string account;
    mutable messaging::sender atm_queue;

    get_balance(std::string _account, const messaging::sender _atm_queue):
        account(std::move(_account)), atm_queue(_atm_queue) {}
};

struct balance {
    unsigned amount;

    explicit balance(const unsigned _amount):
        amount(_amount){}
};

struct display_balance {
    unsigned amount;
    explicit display_balance(const unsigned _amount):
        amount(_amount){}
};

struct balance_pressed
{};