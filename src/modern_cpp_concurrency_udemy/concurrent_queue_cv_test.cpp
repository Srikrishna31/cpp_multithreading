#include "concurrent_queue_cv.h"
#include <future>
#include <iostream>
#include <string>

concurrent_queue_cv<std::string> conc_que;

auto reader() -> void {
    auto data = std::string{};
    std::cout << "Reader calling pop..." << std::endl;
    for (int i = 0; i < 60; ++i) {
        conc_que.pop(data);
        std::cout << "Reader received data: " << data << std::endl;
    }
}

auto writer() -> void {
    using namespace std::literals;

    std::this_thread::sleep_for(2s);
    std::cout << "Writer calling push..." << std::endl;
    for (int i = 0; i < 60; ++i) {
        conc_que.push("Item " + std::to_string(i));
    }
}

auto main() -> int {
    std::cout << "Starting reader" << std::endl;
    auto read_fut = std::async(std::launch::async, reader);

    std::cout << "Starting writer" << std::endl;
    auto write_fut = std::async(std::launch::async, writer);

    read_fut.wait();
    write_fut.wait();

    return 0;
}
