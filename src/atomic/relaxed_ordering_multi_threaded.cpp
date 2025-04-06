//
// Created by coolk on 05-04-2025.
//

#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> x{0}, y{0}, z{0};  // <--- (1)
std::atomic<bool> go {false};                 // <--- (2)

constexpr unsigned loop_count = 10;

struct read_values {
    int x, y, z;
};

read_values values1[loop_count];
read_values values2[loop_count];
read_values values3[loop_count];
read_values values4[loop_count];
read_values values5[loop_count];

void increment(std::atomic<int>* var_to_inc, read_values* values) {
    while (!go)             // <--- (3) Spin, waiting for the signal
        std::this_thread::yield();
    for (auto i = 0; i < loop_count; ++i) {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        var_to_inc->store(i+1, std::memory_order_relaxed);  // <--- (4)
        std::this_thread::yield();
    }
}

void read_vals(read_values* values) {
    while (!go)         // <--- (5) Spin, waiting for the signal
        std::this_thread::yield();
    for (auto i = 0; i < loop_count; ++i) {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void print_values(const read_values* v) {
    for (auto i = 0; i < loop_count; ++i) {
        if (i)
            std::cout << ", ";
        std::cout << "( " << v->x << ", " << v->y << ", " << v->z << " )";
    }
    std::cout << std::endl;
}

/**
 *
 * We have three shared global atomic variables (1) and five threads. Each thread loops 10 times, reading the values of
 * the three atomic variables using memory_order_relaxed and storing them in an array. Three of the threads each update
 * one of the atomic variables each time through the loop (4), while the other two threads just read. Once all threads
 * have been joined, we print the values from the arrays stored by each thread (7).
 * The atomic variable go (2) is used to ensure that the threads all start the loop as near to the same time as possible.
 * Launching a thread is an expensive operation, and without the explicit delay, the first thread may be finished before
 * the last one has started. Each thread waits for go to become true before entering the main loop (3), (5) and go is set
 * to true only once all the threads have started (6).
 */
int main() {
    std::thread t1(increment, &x, values1);
    std::thread t2(increment, &y, values2);
    std::thread t3(increment, &z, values3);
    std::thread t4(read_vals, values4);
    std::thread t5(read_vals, values5);

    go = true;      // <--- (6) Signal to start execution of main loop

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    print_values(values1);      // <--- (7) Print the final values
    print_values(values2);
    print_values(values3);
    print_values(values4);
    print_values(values5);
}