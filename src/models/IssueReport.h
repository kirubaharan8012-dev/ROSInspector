#ifndef ISSUEREPORT_H
#define ISSUEREPORT_H

#include <QString>

enum class IssueSeverity {
    Info,
    Warning,
    Critical
};

struct IssueReport {
    QString filePath;
    int lineNumber;
    QString message;
    QString suggestedFix;
    IssueSeverity severity;
};

#endif // ISSUEREPORT_H