#ifndef CROSSFILEANALYZER_H
#define CROSSFILEANALYZER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariantMap>
#include <QVariantList>
#include <QMap>
#include "../models/IssueReport.h"

struct GraphEdge {
    QString fromNode;
    QString toNode;
    int confidence;    // 100 = Direct Green, 60 = Package Yellow, 0 = Isolated
    QString edgeLabel; // "Direct Call", "Package Association", etc.
};

class CrossFileAnalyzer {
public:
    static QVector<IssueReport> analyzeCrossReferences(const QStringList &filePaths);

    // Incremental Multi-Level Dependency Graph Generator
    static QVariantMap generateVirtualGraph(const QStringList &filePaths, const QMap<QString, int> &scoresMap);
};

#endif // CROSSFILEANALYZER_H