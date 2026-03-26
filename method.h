#ifndef METHOD_H
#define METHOD_H

#include <QString>

class Method {
public:
    virtual ~Method() {}
    virtual QString execute(QString input) = 0;
};

#endif