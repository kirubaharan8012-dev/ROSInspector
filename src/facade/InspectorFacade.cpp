#include "InspectorFacade.h"
#include "../parsers/XmlParser.h"
#include "../parsers/CmakeParser.h"
#include "../parsers/YamlParser.h"
#include "../parsers/UrdfParser.h"
#include "../parsers/LaunchParser.h"
#include "../engines/CrossFileAnalyzer.h"
#include "../engines/HealthCalculator.h"

#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

InspectorFacade::InspectorFacade(QObject *parent) : QObject(parent) {}

// 🟢 QML-ல் இருந்து பல ஃபைல்கள் (Urls Array) வரும்போது அதை ஒவ்வொன்றாகப் பிரித்துச் சேர்க்கும் முறை
void InspectorFacade::inspectFiles(const QVariantList &fileUrls) {
    for (const QVariant &urlVar : fileUrls) {
        addDroppedFile(urlVar.toString());
    }
}

void InspectorFacade::addDroppedFile(const QString &fileUrl) {
    QString filePath = QUrl(fileUrl).toLocalFile();
    if (filePath.isEmpty()) filePath = fileUrl;

    QFileInfo info(filePath);
    QString fileName = info.fileName();

    QVector<IssueReport> issuesList;

    if (fileName.endsWith(".urdf") || fileName.endsWith(".xacro")) {
        issuesList = UrdfParser::parseUrdfXacro(filePath);
    } else if (fileName == "package.xml") {
        issuesList = XmlParser::parsePackageXml(filePath);
    } else if (fileName == "CMakeLists.txt") {
        issuesList = CmakeParser::parseCMakeLists(filePath);
    } else if (fileName.endsWith(".yaml") || fileName.endsWith(".yml")) {
        issuesList = YamlParser::parseYamlParams(filePath);
    } else if (fileName.endsWith(".launch.py") || fileName.endsWith(".py")) {
        issuesList = LaunchParser::parseLaunchPy(filePath);
    }

    int score = HealthCalculator::calculateScore(issuesList);
    m_fileIssuesMap[filePath] = issuesList;
    m_fileScoresMap[filePath] = score;

    recalculateOverall();
    selectFileForInspection(filePath);
}

void InspectorFacade::selectFileForInspection(const QString &filePath) {
    m_currentSelectedPath = filePath;
    QVector<IssueReport> issuesList = m_fileIssuesMap.value(filePath);

    QVariantList qmlIssues;
    for (const auto &issue : issuesList) {
        QVariantMap map;
        map["filePath"] = issue.filePath;
        map["lineNumber"] = issue.lineNumber;
        map["message"] = issue.message;
        map["suggestedFix"] = issue.suggestedFix;
        map["severity"] = static_cast<int>(issue.severity);
        qmlIssues.append(map);
    }

    m_selectedIssues = qmlIssues;
    emit issuesChanged();
}

void InspectorFacade::applyAutoFix(const QString &filePath, int lineNumber, const QString &message) {
    Q_UNUSED(lineNumber);
    
    if (message.contains("ros__parameters")) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = file.readAll();
            file.close();

            if (!content.contains("ros__parameters:")) {
                QStringList lines = content.split("\n");
                if (!lines.isEmpty()) {
                    QString updatedContent = lines[0] + "\n  ros__parameters:\n";
                    for (int i = 1; i < lines.size(); ++i) {
                        updatedContent += "    " + lines[i] + "\n";
                    }

                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&file);
                        out << updatedContent;
                        file.close();
                    }
                }
            }
        }
    }

    addDroppedFile(filePath);
}

void InspectorFacade::recalculateOverall() {
    QVariantList newList;
    QVector<IssueReport> allIssues;
    QStringList allPaths;

    for (auto it = m_fileScoresMap.begin(); it != m_fileScoresMap.end(); ++it) {
        QString path = it.key();
        QFileInfo info(path);

        QVariantMap map;
        map["fileName"] = info.fileName();
        map["filePath"] = path;
        map["score"] = it.value();
        newList.append(map);

        allIssues.append(m_fileIssuesMap[path]);
        allPaths.append(path);
    }

    m_fileListModel = newList;
    m_overallScore = HealthCalculator::calculateScore(allIssues);

    m_graphData = CrossFileAnalyzer::generateVirtualGraph(allPaths, m_fileScoresMap);

    emit fileListModelChanged();
    emit overallScoreChanged();
    emit graphDataChanged();
}