//
// Created by coolk on 25-05-2025.
//

/**
 * Dining Philosophers Rules
 *  -> A philosopher has two states: thinking and eating
 *  -> Each fork can only be held by one philosopher at a time.
 *      - A philosopher can only pick up one fork at a time
 *      - A philosopher must pick up both forks before they can eat.
 *      - When a philosopher finishes eating, they put down both forks immediately
 *      - A philosopher may pick up a fork as soon as it is put down by another.
 *  -> A philosopher as no awareness of other philosopher's actions
 *  -> If a philosopher doesn't eat at all, they will die of starvation
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>

using namespace std::literals;

// Some data about the problem
constexpr int nforks = 5;
constexpr int nphilosophers = nforks;
std::string names[nphilosophers] = {"A", "B", "C", "D", "E"};

// Keep track of how many times a philosopher is able to eat
int mouthfuls[nphilosophers] = {0};

// A philosopher who has not picked up both forks is thinking
constexpr auto think_time = 2s;

// A philosopher who has picked up both forks is eating
constexpr auto eat_time = 1s;

// A mutex prevents more than one philosopher picking up the same fork
// A philosopher thread can only pick up a fork if it can lock the corresponding mutex.
std::mutex fork_mutex[nforks];

// Mutex to protect the output
std::mutex print_mutex;

// Functions to display information about the 'nth' philosopher

// Interactions with a fork
auto print(int n, const std::string& str, int forkno) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher #" << names[n] << str << forkno << std::endl;
}

// The philosopher's state
auto print(int n, const std::string& str) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher #" << names[n] << str << std::endl;
}

// Thread which represents a dining philosopher
auto dint(const int nphilo) {
    /**
     * Philosopher A has fork 0 on their left and fork 1 on their right
     * Philosopher B has fokr 1 on their left and fork 2 on their right
     * ...
     * Philospher E has fokr 4 on their left and fork 0 on their right
     *
     * Each philosopher must pick up their left fork first
     */
    const int lfork = nphilo;
    const int rfork = (nphilo + 1) % nforks;

    print(nphilo, "\'s left fork is number", lfork);
    print(nphilo, "\'s right fork is number", rfork);
    print(nphilo, " is thinking...");

    std::this_thread::sleep_for(think_time);

    // Make an attempt to eat
    print(nphilo, " reaches for fork number ", lfork);

    // Try to pick up the left fork
    fork_mutex[lfork].lock();
    print(nphilo, " picks up fork ", lfork);
    print(nphilo, " is thinking...");

    // Succeeded - now try to pick up the right fork
    std::this_thread::sleep_for(think_time);

    print(nphilo, " reaches for fork number ", rfork);

    fork_mutex[rfork].lock();

    //Succeeded - this philosopher can now eat
    print(nphilo, " picks up fork ", rfork);
    print(nphilo, " is eating...");

    std::this_thread::sleep_for(eat_time);

    print(nphilo, " puts down fork ", lfork);
    print(nphilo, " puts down fork ", rfork);
    print(nphilo, " is thinking...");

    fork_mutex[lfork].unlock();
    fork_mutex[rfork].unlock();
    std::this_thread::sleep_for(think_time);
}

auto main() -> int {

    // Start a separate thread for each philosopher
    std::vector<std::thread> philosophers(nphilosophers);

    for (int i = 0; i < nphilosophers; i++) {
        philosophers[i] = std::thread(dint, i);
    }

    for (auto& philo : philosophers) {
        philo.join();
    }

    // How many times were the philosophers were able to eat?
    for (int i = 0; i < nphilosophers; i++) {
        std::cout << "Philosopher #" << names[i] << " had " << mouthfuls[i] << "mouthfuls" << std::endl;
    }

    return 0;
}