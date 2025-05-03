# pragma once

#include <algorithm>
#include <list>

/**
 *
 * You take the first element as the pivot by slicing it off the front of the list using splice() (1).
 */
template <typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> output;
    output.splice(output.begin(), input, input.begin());      // <--- (1)
    T const& pivot = *output.begin();       // <--- (2)

    auto divide_point =
        std::partition(input.begin(), input.end(),[&](T const& t) {return t<pivot;}); // <--- (3)

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);    // <--- (4)

    auto new_lower(sequential_quick_sort(std::move(lower_part)));   // <--- (5)
    auto new_higher(sequential_quick_sort(std::move(input)));   // <--- (6)

    output.splice(output.end(), new_higher);        // <--- (7)
    output.splice(output.begin(), new_lower);       // <--- (8)

    return output;
}
