#ifndef URDFPARSER_H
#define URDFPARSER_H

#include <QString>
#include <QVector>
#include "../models/IssueReport.h"

class UrdfParser {
public:
    static QVector<IssueReport> parseUrdfXacro(const QString &filePath);
};

#endif // URDFPARSER_H