#ifndef LAUNCHPARSER_H
#define LAUNCHPARSER_H

#include <QString>
#include <QVector>
#include "../models/IssueReport.h"

class LaunchParser {
public:
    static QVector<IssueReport> parseLaunchPy(const QString &filePath);
};

#endif // LAUNCHPARSER_H