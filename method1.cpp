#include "method1.h"
#include <QStringList>  // <-- ÝÒÎ ÍÓÆÍÎ ÄÎÁÀÂÈÒÜ!
#include <cmath>

double eval1(QString func, double x) {
    if (func.contains("x^3")) return x * x * x - x - 2;
    if (func.contains("x^2")) return x * x - 4;
    if (func.contains("sin")) return sin(x);
    return 0;
}

QString Method1::execute(QString input) {
    QStringList p = input.split(' ', Qt::SkipEmptyParts);
    if (p.size() < 3) return "Error: need a b func";

    bool ok1, ok2;
    double a = p[0].toDouble(&ok1);
    double b = p[1].toDouble(&ok2);
    if (!ok1 || !ok2) return "Error: a,b must be numbers";

    QString func;
    for (int i = 2; i < p.size(); i++) func += p[i] + " ";

    double fa = eval1(func, a);
    double fb = eval1(func, b);

    if (fa * fb > 0) return "Error: different signs required";

    double c = (a + b) / 2;
    double fc = eval1(func, c);

    double na, nb;
    if (fa * fc < 0) { na = a; nb = c; }
    else { na = c; nb = b; }

    return QString("New interval: [%1, %2]").arg(na).arg(nb);
}