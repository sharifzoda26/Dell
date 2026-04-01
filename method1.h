#ifndef METHOD1_H
#define METHOD1_H

#include "method.h"
#include <cmath>

class Method1 : public Method {
public:
    QString execute(QString input) override;
};

#endif