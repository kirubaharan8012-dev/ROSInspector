#ifndef YAMLPARSER_H
#define YAMLPARSER_H

#include <QString>
#include <QVector>
#include "../models/IssueReport.h"

class YamlParser {
public:
    static QVector<IssueReport> parseYamlParams(const QString &filePath);
};

#endif // YAMLPARSER_H