#include <mutex>
#include <iostream>
#include <string>
#include <vector>
#include <future>

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

// Monitor class - can wrap any type of object
template<typename T>
class Monitor
{
    // The object to be monitored
    T data;
    std::mutex mut; // Mutex to protect it.

    public:
    Monitor<T>(T data = T{}) : data(data) {} // Default constructor.

    template<typename Func>
    // Argument is a callable object of type Func that takes an argument of type T&
    auto operator()(Func func) {
        std::lock_guard<std::mutex> lock(mut);
        return func(data);
    }
};
/**
 * Sophisticated Monitor Class
 *  - We make it generic
 *      * The wrpped class's type is the template parameter T
 *  - Functor class with overloaded operator()
 *      * The argument is a callable object.
 *      * This contains the sequence of member function calls for the transaction.
 *      * We lock the mutex, then invoke the callable object.
 *  - This will be a template member function
 *      * The type parameter is the type of the callable object.
 *
 *  - Advantages of Sophisticated Monitor Class
 *      * Works with any type.
 *         - Including classes that were not designed for threaded code.
 *         - No modifications required to the wrapped class.
 *      * Allows callers to perform transactions efficiently and safely.
 *         - Avoids unnecessary locking and unlocking.
 *         - Avoids multiple locking and unlocking.
 *         - Prevents interruptions by other threads.
 *
 *      * Gives callers complete freedom
 *         - Which public members to call in a transaction.
 *         - What order to call them in.
 *
 */
auto main() -> int {
    Monitor<Bank> mon;

    // Invoke its function call operator and pass a callable object which takes a bank argument
    mon([](Bank& bank) {
        bank.debit("Alice", 100);
        bank.credit("Bob", 50);
        bank.print("Alice");
    });

    std::cout << "Done" << std::endl;

    using namespace std::string_literals;

    // Monitor wrapper for string
    Monitor<std::string> mon_str("start: "s);

    for (int i = 0; i < 5; ++i) {
        // Invoke its function call operator and pass a callable object which takes a string argument
        mon_str([i](std::string& str) {
            str += std::to_string(i) + ", " + std::to_string(i) + ", ";
        });

        // Invoke the function call operator again
        mon_str([](std::string& str) {
            std::cout << str << "\n";
        });
    }

    std::cout << "Done" << std::endl;

    Monitor<std::string> mon_str2("start: "s);
    auto futs = std::vector<std::future<void>>{};

    for (int i = 0; i < 5; ++i) {
        futs.push_back(std::async([&mon_str2, i] {
            mon_str2([i](std::string& str) {
                str += std::to_string(i) + ", " + std::to_string(i) + ", ";
            });
            mon_str2([](std::string& str) {
                std::cout << str << "\n";
            });
        }));
    }

    for (auto& fut : futs) {
        fut.wait();
    }
    return 0;
}
