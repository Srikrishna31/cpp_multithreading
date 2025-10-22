
// A naive internally synchronized Bank class
// Has a data member which is a mutex.
// Each member function locks the mutex when called.


#include <iostream>
#include <mutex>
#include <thread>

// Very simple Bank class
class Bank {
    // Mutex to protect the balance
    std::mutex mut;
    public:
        void debit(const std::string& name, int amount) {
            std::lock_guard<std::mutex> lock(mut);
            std::cout << "Debiting " << name << " by " << amount << std::endl;
        }
        void credit(const std::string& name, int amount) {
            std::lock_guard<std::mutex> lock(mut);
            std::cout << "Crediting " << name << " by " << amount << std::endl;
        }
        void print(const std::string& name) {
            std::lock_guard<std::mutex> lock(mut);
            std::cout << "Printing balance of " << name << std::endl;
        }
};

/**
 * Naive Solution drawbacks:
 *  - Member functions may need to call other member functions
 *      * Using multiple locks risks deadlocks
 *  - Transactions may involve multiple member function calls
 *  - Results in many locking and unlocking operations
 *      * Race conditions
 *      * Potential data race
 *  - Existing classes need to be modified to use the monitor pattern
 */
auto main() -> int {
    Bank bank;
    std::thread thr ([&bank](){
        bank.debit("Alice", 100);
        bank.credit("Bob", 200);
        bank.print("Alice");
        bank.print("Bob");
    });
    thr.join();
    std::cout << "Done" << std::endl;
    return 0;
}
