#ifndef METHOD_FACTORY_H
#define METHOD_FACTORY_H

#include "method.h"
#include "method1.h"
#include "method_stub.h"

class MethodFactory {
public:
    static Method* get(int variant) {
        
        if (variant == 1) return new Method1();
        return new MethodStub(variant);
    }
};

#endif