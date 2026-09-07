#ifndef INSPECTORFACADE_H
#define INSPECTORFACADE_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QMap>
#include "../models/IssueReport.h"

class InspectorFacade : public QObject {
    Q_OBJECT
    Q_PROPERTY(int overallScore READ overallScore NOTIFY overallScoreChanged)
    Q_PROPERTY(QVariantList fileListModel READ fileListModel NOTIFY fileListModelChanged)
    Q_PROPERTY(QVariantList issues READ issues NOTIFY issuesChanged)
    Q_PROPERTY(QVariantMap graphData READ graphData NOTIFY graphDataChanged)

public:
    explicit InspectorFacade(QObject *parent = nullptr);

    int overallScore() const { return m_overallScore; }
    QVariantList fileListModel() const { return m_fileListModel; }
    QVariantList issues() const { return m_selectedIssues; }
    QVariantMap graphData() const { return m_graphData; }

    // 🟢 QML UI-க்கு தேவையான புதிய inspectFiles மற்றும் பழைய addDroppedFile
    Q_INVOKABLE void inspectFiles(const QVariantList &fileUrls);
    Q_INVOKABLE void addDroppedFile(const QString &fileUrl);
    Q_INVOKABLE void selectFileForInspection(const QString &filePath);
    Q_INVOKABLE void applyAutoFix(const QString &filePath, int lineNumber, const QString &message);

signals:
    void overallScoreChanged();
    void fileListModelChanged();
    void issuesChanged();
    void graphDataChanged();

private:
    void recalculateOverall();

    int m_overallScore = 100;
    QVariantList m_fileListModel;
    QVariantList m_selectedIssues;
    QVariantMap m_graphData;
    
    QMap<QString, QVector<IssueReport>> m_fileIssuesMap;
    QMap<QString, int> m_fileScoresMap;
    QString m_currentSelectedPath;
};

#endif // INSPECTORFACADE_H