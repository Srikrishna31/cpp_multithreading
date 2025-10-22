#include <mutex>
#include <iostream>

class Bank {
    // Shared data...
    public:
        void debit(const std::string& name, int amount) {
            std::cout << "Debiting " << name << " by " << amount << std::endl;
        }
        void credit(const std::string& name, int amount) {
            std::cout << "Crediting " << name << " by " << amount << std::endl;
        }
        void print(const std::string& name) {
            std::cout << "Printing balance of " << name << std::endl;
        }
};

class BankMonitor{
    std::mutex mut;
    Bank bank;

    public:
        void debit(const std::string& name, int amount){
            std::lock_guard<std::mutex> lock(mut);
            bank.debit(name, amount);
        }

        void credit(const std::string& name, int amount){
            std::lock_guard<std::mutex> lock(mut);
            bank.credit(name, amount);
        }

        void print(const std::string& name){
            std::lock_guard<std::mutex> lock(mut);
            bank.print(name);
        }
};

/**
 * Monitor class:
 *  - We write a wrapper class
 *      * The bank object is a data member of the monitor class
 *      * The member functions lock a mutex and forward to the bank object.
 *  - Works with any type
 *      * Including classes that were not designed with concurrency in mind.
 *      * No modifications needed to the wrapped class.
 *  - Does not help callers who want to perform transactions
 *      * Unnecessary locking
 *      * Possibility of deadlock due to multiple locking
 *      * Allows interruptions by other threads.
 *
 */

auto main() -> int {
    BankMonitor monitor;

    std::thread thr([&monitor]{
        monitor.debit("Alice", 100);
        monitor.credit("Bob", 50);
        monitor.print("Alice");
        monitor.print("Bob");
    });

    thr.join();

    std::cout << "Done" << std::endl;

    return 0;
}
