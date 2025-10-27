#include "odr.h"

// The code in this folder is taken from wikipedia, to demonstrate the issue of ODR:
// https://en.wikipedia.org/wiki/One_Definition_Rule
auto main() -> int {
    CBase* o1 = odr1Create();
    CBase* o2 = odr2Create();

    o1->myFunc();
    o2->myFunc();

    delete o1;
    delete o2;
    return 0;
}
