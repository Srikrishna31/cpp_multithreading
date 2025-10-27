#pragma once

class CBase {
    public:
        virtual void myFunc() = 0;
        virtual ~CBase() = default;
};

extern CBase* odr1Create();
extern CBase* odr2Create();
