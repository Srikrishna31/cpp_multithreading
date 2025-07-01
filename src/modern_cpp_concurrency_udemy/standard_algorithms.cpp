//
// Created by coolk on 01-07-2025.
//
#include <algorithm>
#include <string>
#include <iostream>
#include <ranges>
using namespace std::literals;
/**
*  Standard Algorithms
*      # A set of functions in the Standard Library
*          * Implement classic algorithms, such as searching and sorting
*          * Plus populating, copying, reordering etc.
*          * Operate on containers and sequences of data
*     # Most are in <algorithm>, few are in <numeric>.
*     # Algorithm Execution
*          * Function call which takes an iterator range
*              - Usually corresponds to a sequence of elements in a container
*              - Often begin() and end(), to process the entire container
*          * Iterates over the range of elements
*          * Performs an operation on the elements
*          * Returns either
*              * An iterator representing an element of interest
*              * The result of the operation on the elements.
*    # Predicates
*          * Many algorithms use a "predicate"
*               - A function which returns a bool
*          * std::find() uses the == operator
*               - Compares each element to the target value
*          * std::find_if() allows us to supply our own predicate
*               - Pass callable object as an extra argument
*               - Allows us to change the definition of equality
*          * The predicate
*               - Takes an argument of the element type, and returns a bool.
*/
auto main() -> int {
    auto str = "Hello World!"s;
    std::cout << "String to search: " << str << std::endl;

    std::cout << "Searching for first occurrence of \'o\'" << std::endl;
    // auto res = std::find(str.begin(), str.end(), 'o');
    auto res = std::find_if(str.cbegin(), str.cend(), [](const char c) { return ::toupper(c) == 'O';});

    if (res != str.cend()) {
        std::cout << "Found a matching element at index: " << res - str.begin() << std::endl;

        std::cout << "At this point in the string: ";
        // for (auto it = res; it != str.cend(); ++it) {
        for (const auto ch: std::ranges::subrange(res, str.end())){
            std::cout << ch;
        }
        std::cout << std::endl;
    } else {
        std::cout << "Not found" << std::endl;
    }

    return 0;
}