#include "odr.h"
#include <iostream>

class CDummy: public CBase {
    public:
        void myFunc() override {
            std::cout << "odr ONE dummy: Hello" << std::endl;
        }
};

CBase* odr1Create() {
    return new CDummy();
}
