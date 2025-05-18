//
// Created by coolk on 17-05-2025.
//

#include <iostream>
#include <string>
#include <thread>

auto fizzbuzz(const uint32_t val) -> std::string {
    const auto div_by_3 = val % 3 == 0;
    const auto div_by_5 = val % 5 == 0;

    if (div_by_3 && div_by_5) {
        return "fizzbuzz";
    }
    if (div_by_5) {
        return "buzz";
    }
    if (div_by_3) {
        return "fizz";
    }

    std::cout << val << std::endl;
    return std::to_string(val);
}

int main () {
    std::thread t1(fizzbuzz, 10);
    std::thread t2(fizzbuzz, 15);

    t1.join();
    t2.join();

    return 0;
    // std::cout << fizzbuzz(1) << std::endl;
    // std::cout << fizzbuzz(2) << std::endl;
    // std::cout << fizzbuzz(3) << std::endl;
    // std::cout << fizzbuzz(4) << std::endl;
    // std::cout << fizzbuzz(5) << std::endl;
    // std::cout << fizzbuzz(6) << std::endl;
    // std::cout << fizzbuzz(7) << std::endl;
    // std::cout << fizzbuzz(8) << std::endl;
    // std::cout << fizzbuzz(15) << std::endl;
}