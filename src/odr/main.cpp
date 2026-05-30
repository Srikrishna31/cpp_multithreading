#include "odr.h"
#include <type_traits>
#include <iostream>

// The code in this folder is taken from wikipedia, to demonstrate the issue of ODR:
// https://en.wikipedia.org/wiki/One_Definition_Rule
auto main() -> int {
    CBase* o1 = odr1Create();
    CBase* o2 = odr2Create();

    o1->myFunc();
    o2->myFunc();

    delete o1;
    delete o2;

    std::cout << std::boolalpha;
    std::cout << "int: " << std::is_arithmetic_v<int> << "\n";       // true
    std::cout << "double: " << std::is_arithmetic<double>::value << "\n"; // true
    std::cout << "std::string: " << std::is_arithmetic<std::string>::value << "\n"; // false
    std::cout << "boolean: " << std::is_arithmetic<bool>::value << "\n";  // false

    return 0;
}
