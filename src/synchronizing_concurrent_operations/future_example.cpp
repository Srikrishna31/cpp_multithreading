//
// Created by coolk on 16-04-2025.
//
#include <future>
#include <iostream>
#include <thread>

int find_the_answer_to_ltuae() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 42;
}

void do_other_stuff() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

struct X {
    void foo(int, std::string const &) {}
    std::string bar(std::string const& ) {return "from bar";}
};

struct Y {
    double operator () (const double val) const {
        std::cout << "Value passed: " << val << std::endl;
        return 3.14;
    }
};

class move_only {
public:
    move_only() = default;
    move_only(move_only&& ) = default;
    move_only& operator=(move_only&& ) = default;
    move_only(move_only const&) = delete;
    move_only& operator=(move_only const&) = delete;

    void operator () () const { std::cout << "move only () operator called." << std::endl; }
};

X baz(X&x) {
    x.bar("");
    x.foo(find_the_answer_to_ltuae(), "");
    return x;
}

/**
 *
 * Waiting for one-off events with futures
 *
 * The C++ standard library models this sort of one-off event with something called a future. If a thread needs to wait
 * for a specific one-off event it somehow obtains a future representing this event. The thread can then periodically
 * wait on the future for short periods of time to see if the event has occurred while performing some other task in
 * between checks. Alternatively, it can do another task until it needs the event to have happened before it can proceed
 * and then just wait for the future to become ready. A future may have data associated with it, or it may not. Once an
 * event has happened, the future can't be reset.
 * There are two sorts of futures in the C++ Standard Library, implemented as two class templates declared in the <future>
 * library header: unique futures (std::future<>) and shared futures (std::shared_future<>). These are modeled after
 * std::unique_ptr and std::shared_ptr. An instance of std::future is the one and only instance that refers to its
 * associated event, whereas multiple instances of std::shared_future may refer to the same event. In the latter case,
 * all the instances will become ready at the same time, and they may all access any data associated with the event. This
 * associated data is the reason these are template; just like std::unique_ptr and std::shared_ptr, the template parameter
 * is the type of the associated data. The std::future<void>, std::shared_future<void> template specializations should be
 * used where there's no associated data. Although futures are used to communicate between threads, the future objects
 * themselves don't provide synchronized access. If multiple threads need to access a single future object, they must
 * protect access via a mutex or other synchronization mechanism. However, multiple threads may each access their own
 * copy of a std::shared_future<> without further synchronization, even if they all refer to the same asynchronous result.
 */
int main() {
    std::future<int> f1 = std::async(find_the_answer_to_ltuae);
    do_other_stuff();
    std::cout << "The answer is " << f1.get() << std::endl;

    X x;
    auto f2 = std::async(&X::foo, &x, 42, "hello");  // <--- Calls p->foo(42, "hello"), where p is &x.
    auto f3 = std::async(&X::bar, x, "goodbye"); // <--- Calls tmpx.bar("goodbye") where tmpx is a copy of x.

    Y y;
    auto f4 = std::async(Y(), 3.141);   // <--- Calls tmpy(3.14) where tmpy is move-constructed from Y()
    auto f5 = std::async(std::ref(y), 2.718);   // <--- Calls y(2.718)

    auto f6 = std::async(baz, std::ref(x)); // Calls baz(x)

    auto f7 = std::async(move_only());  // <--- Calls tmp() where tmp is constructed from std::move(move_only())
}
