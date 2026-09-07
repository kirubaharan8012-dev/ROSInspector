#include "CrossFileAnalyzer.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <tinyxml2.h>
#include <yaml-cpp/yaml.h>

using namespace tinyxml2;

QVector<IssueReport> CrossFileAnalyzer::analyzeCrossReferences(const QStringList &filePaths) {
    QVector<IssueReport> issues;
    QString packageXmlPath;
    QString cmakePath;
    QVector<QString> xacroFrames;
    QVector<QString> yamlFrames;

    for (const QString &filePath : filePaths) {
        if (filePath.endsWith("package.xml")) {
            packageXmlPath = filePath;
        } else if (filePath.endsWith("CMakeLists.txt")) {
            cmakePath = filePath;
        } else if (filePath.endsWith(".urdf") || filePath.endsWith(".xacro")) {
            XMLDocument doc;
            if (doc.LoadFile(filePath.toUtf8().constData()) == XML_SUCCESS) {
                XMLElement *robot = doc.FirstChildElement("robot");
                if (robot) {
                    for (XMLElement *link = robot->FirstChildElement("link"); link; link = link->NextSiblingElement("link")) {
                        const char *name = link->Attribute("name");
                        if (name) xacroFrames.append(QString(name));
                    }
                }
            }
        } else if (filePath.endsWith(".yaml") || filePath.endsWith(".yml")) {
            try {
                YAML::Node node = YAML::LoadFile(filePath.toUtf8().constData());
                QFile yFile(filePath);
                if (yFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&yFile);
                    QString content = stream.readAll();
                    QRegularExpression regex(R"(\b\w*_frame\b)");
                    QRegularExpressionMatchIterator matchIt = regex.globalMatch(content);
                    while (matchIt.hasNext()) {
                        yamlFrames.append(matchIt.next().captured(0));
                    }
                }
            } catch (...) {}
        }
    }

    if (!packageXmlPath.isEmpty() && !cmakePath.isEmpty()) {
        QFile cFile(cmakePath);
        if (cFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString cmakeContent = QString::fromUtf8(cFile.readAll());
            if (cmakeContent.contains("find_package(rclcpp") || cmakeContent.contains("rclcpp")) {
                QFile pFile(packageXmlPath);
                if (pFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString pkgContent = QString::fromUtf8(pFile.readAll());
                    if (!pkgContent.contains("<depend>rclcpp</depend>") && 
                        !pkgContent.contains("<exec_depend>rclcpp</exec_depend>")) {
                        issues.append({
                            packageXmlPath,
                            1,
                            "Mismatch: CMakeLists.txt uses 'rclcpp', but it is missing in package.xml",
                            "Add <depend>rclcpp</depend> inside package.xml.",
                            IssueSeverity::Critical
                        });
                    }
                }
            }
        }
    }

    return issues;
}

QVariantMap CrossFileAnalyzer::generateVirtualGraph(const QStringList &filePaths, const QMap<QString, int> &scoresMap) {
    QVariantList nodesList;
    QVariantList edgesList;

    QString detectedPackageName = "";

    // 1. First Pass: Detect Package Name from package.xml or CMakeLists.txt
    for (const QString &path : filePaths) {
        QFileInfo info(path);
        if (info.fileName() == "package.xml") {
            XMLDocument doc;
            if (doc.LoadFile(path.toUtf8().constData()) == XML_SUCCESS) {
                XMLElement *nameElem = doc.FirstChildElement("package") ? doc.FirstChildElement("package")->FirstChildElement("name") : nullptr;
                if (nameElem && nameElem->GetText()) {
                    detectedPackageName = QString(nameElem->GetText()).trimmed();
                }
            }
        } else if (info.fileName() == "CMakeLists.txt" && detectedPackageName.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString content = file.readAll();
                QRegularExpression regex(R"(project\s*\(\s*(\w+)\s*\))");
                QRegularExpressionMatch match = regex.match(content);
                if (match.hasMatch()) {
                    detectedPackageName = match.captured(1);
                }
            }
        }
    }

    // 2. Second Pass: Build Nodes & Detect Isolation / Workspace Match
    for (const QString &path : filePaths) {
        QFileInfo info(path);
        QDir dir = info.absoluteDir();

        QVariantMap node;
        node["id"] = path;
        node["label"] = info.fileName();
        node["folder"] = dir.dirName();
        node["score"] = scoresMap.value(path, 100);

        // Package matching check
        bool belongsToPackage = true;
        if (!detectedPackageName.isEmpty()) {
            if (!path.contains(detectedPackageName) && dir.dirName() != detectedPackageName) {
                belongsToPackage = false; // Foreign Workspace file
            }
        }
        node["isForeign"] = !belongsToPackage;

        if (info.fileName().endsWith(".launch.py")) node["type"] = "LAUNCH";
        else if (info.fileName().endsWith(".urdf") || info.fileName().endsWith(".xacro")) node["type"] = "URDF";
        else if (info.fileName().endsWith(".yaml") || info.fileName().endsWith(".yml")) node["type"] = "CONFIG";
        else if (info.fileName() == "package.xml" || info.fileName() == "CMakeLists.txt") node["type"] = "BUILD";
        else node["type"] = "OTHER";

        nodesList.append(node);
    }

    // 3. Third Pass: Incremental Dependency & Confidence Graph Matching
    for (int i = 0; i < filePaths.size(); ++i) {
        QFile file(filePaths[i]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        
        QString content = file.readAll();
        file.close();
        QFileInfo srcInfo(filePaths[i]);

        for (int j = 0; j < filePaths.size(); ++j) {
            if (i == j) continue;

            QFileInfo targetInfo(filePaths[j]);
            
            // A) Direct Match (100% High Confidence - Green Solid Edge)
            if (content.contains(targetInfo.fileName())) {
                QVariantMap edge;
                edge["from"] = filePaths[i];
                edge["to"] = filePaths[j];
                edge["confidence"] = 100;
                edge["label"] = "Direct Reference";
                edgesList.append(edge);
            } 
            // B) Package-Level Association (60% Medium Confidence - Yellow Dashed Edge)
            else if ((srcInfo.fileName() == "package.xml" && targetInfo.fileName() == "CMakeLists.txt") ||
                     (srcInfo.fileName() == "CMakeLists.txt" && targetInfo.fileName() == "package.xml")) {
                if (i < j) { // Avoid duplicate bidirectional edges
                    QVariantMap edge;
                    edge["from"] = filePaths[i];
                    edge["to"] = filePaths[j];
                    edge["confidence"] = 60;
                    edge["label"] = "Package Sync";
                    edgesList.append(edge);
                }
            }
        }
    }

    QVariantMap result;
    result["nodes"] = nodesList;
    result["edges"] = edgesList;
    return result;
}