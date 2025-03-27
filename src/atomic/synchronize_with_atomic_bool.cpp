//
// Created by coolk on 27-03-2025.
//
#include <vector>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

std::vector<int> data;
std::atomic<bool> data_ready{false};

void reader_thread() {
    while (!data_ready.load()) {
        // std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   // <---- (1)
    }
    std::cout << "The answer=" << data[0] << std::endl;    // <----- (2)
}

void writer_thread() {
    data.push_back(42);    // <---- (3)
    data_ready = true;      //  <---- (4)
}

/**
 *
 * Synchronizing operations and enforced ordering
 *
 * The required enforced ordering comes from the operations on the std::atomic<bool> variable data_ready; they provide
 * the necessary ordering by virtue of the memory model relations happens-before and synchronizes-with. The write of the
 * data (3) happens-before the write to the data_ready flag (4), and the read of the flag (1) happens-before the read of
 * the data (2). When the value read from data_ready (1) is true the write-synchronizes with that read, creating a happens-before
 * relationship.
 * Because happens-before is transitive, the write to the data (3) happens-before the write to the flag (4), which
 * happens-before the read of the true value from the flag (1), which happens-before the read of the data (2), and you
 * have an enforced ordering; the write of the data happens-before the read of the data and everything is OK.
 */
int main() {
    std::thread t(reader_thread);
    std::thread t_writer(writer_thread);
    t.join();
    t_writer.join();
}