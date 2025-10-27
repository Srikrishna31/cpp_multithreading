#include "odr.h"
#include <iostream>

class CDummy: public CBase {
    public:
        void myFunc() override {
            std::cout << "odr TWO dummy: World" << std::endl;
        }
};

CBase* odr2Create() {
    return new CDummy();
}
