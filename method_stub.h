#ifndef METHOD_STUB_H
#define METHOD_STUB_H

#include "method.h"

class MethodStub : public Method {
    int num;
public:
    MethodStub(int n) : num(n) {}
    QString execute(QString input) override {
        return QString("Variant %1 not ready. Input: %2").arg(num).arg(input);
    }
};

#endif