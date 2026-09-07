#ifndef CMAKEPARSER_H
#define CMAKEPARSER_H

#include <QString>
#include <QVector>
#include "../models/IssueReport.h"

class CmakeParser {
public:
    static QVector<IssueReport> parseCMakeLists(const QString &filePath);
};

#endif // CMAKEPARSER_H