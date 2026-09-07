#ifndef HEALTHCALCULATOR_H
#define HEALTHCALCULATOR_H

#include <QVector>
#include "../models/IssueReport.h"

class HealthCalculator {
public:
    // Calculates score using the formula: 100 - (Critical*15 + Warning*5 + Info*2)
    static int calculateScore(const QVector<IssueReport> &issues);
};

#endif // HEALTHCALCULATOR_H