#include "HealthCalculator.h"
#include <QtGlobal>

int HealthCalculator::calculateScore(const QVector<IssueReport> &issues) {
    int score = 100;

    for (const auto &issue : issues) {
        switch (issue.severity) {
            case IssueSeverity::Critical:
                score -= 15;
                break;
            case IssueSeverity::Warning:
                score -= 5;
                break;
            case IssueSeverity::Info:
                score -= 2;
                break;
        }
    }

    // Ensure score stays within bounds [0, 100]
    return qBound(0, score, 100);
}