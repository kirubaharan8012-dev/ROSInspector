#include "CmakeParser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

QVector<IssueReport> CmakeParser::parseCMakeLists(const QString &filePath) {
    QVector<IssueReport> issues;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        issues.append({
            filePath,
            0,
            "Unable to open CMakeLists.txt file",
            "Ensure the file exists and has read permissions.",
            IssueSeverity::Critical
        });
        return issues;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 1. Check ament_package() at the end
    if (!content.contains(QRegularExpression(R"(\bament_package\s*\(\s*\))"))) {
        issues.append({
            filePath,
            0,
            "Missing ament_package() call at the end of CMakeLists.txt",
            "Add ament_package() as the last line before closing.",
            IssueSeverity::Critical
        });
    }

    // 2. Check find_package(ament_cmake REQUIRED)
    if (!content.contains(QRegularExpression(R"(\bfind_package\s*\(\s*ament_cmake\b)"))) {
        issues.append({
            filePath,
            1,
            "Missing find_package(ament_cmake REQUIRED)",
            "Add find_package(ament_cmake REQUIRED) near the top.",
            IssueSeverity::Critical
        });
    }

    // 3. Check cmake_minimum_required
    if (!content.contains(QRegularExpression(R"(\bcmake_minimum_required\b)"))) {
        issues.append({
            filePath,
            1,
            "Missing cmake_minimum_required(VERSION ...)",
            "Add cmake_minimum_required(VERSION 3.8) at line 1.",
            IssueSeverity::Warning
        });
    }

    return issues;
}