#include "concurrent_queue.h"
#include <chrono>
#include <iostream>
#include <future>
#include <string>
#include <thread>

concurrent_queue<std::string> conc_que;

// Waiting thread
auto reader() {
    using namespace std::literals;

    std::this_thread::sleep_for(2s);
    auto sdata = std::string{};

    std::cout << "Reader calling pop..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        conc_que.pop(sdata);
        std::cout << "Reader received the data: " << sdata << std::endl;
    }
}

// Modifying thread
auto writer() -> void {
    for (int i = 0; i < 50; ++i) {
        auto sdata = "Item " + std::to_string(i);
        conc_que.push(sdata);
    }

    std::cout << "Writer returned from push" << std::endl;
}

// Main function
int main() {
    auto writer_fut = std::async(std::launch::async, writer);
    auto reader_fut = std::async(std::launch::async, reader);

    try {
        reader_fut.get();
    } catch (std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    try {
        writer_fut.get();
    } catch (std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
