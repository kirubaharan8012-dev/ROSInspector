#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QString>
#include <QVector>
#include "../models/IssueReport.h"

class XmlParser {
public:
    // Pure functional static method: Takes file path -> returns list of issues
    static QVector<IssueReport> parsePackageXml(const QString &filePath);
};

#endif // XMLPARSER_H