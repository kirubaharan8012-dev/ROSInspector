#include "LaunchParser.h"
#include "../models/IssueReport.h"
#include <QFile>
#include <QTextStream>

QVector<IssueReport> LaunchParser::parseLaunchPy(const QString &filePath) {
    QVector<IssueReport> issues;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return issues;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 1. generate_launch_description உள்ளதா எனச் சரிபார்த்தல்
    if (!content.contains("def generate_launch_description()")) {
        IssueReport issue;
        issue.filePath = filePath;
        issue.lineNumber = 1;
        issue.message = "Missing entry point 'def generate_launch_description()'";
        issue.suggestedFix = "Define 'def generate_launch_description():' to make this a valid ROS 2 launch file.";
        issue.severity = IssueSeverity::Critical;
        issues.append(issue);
    }

    // 2. Node Import செய்யப்பட்டுள்ளதா எனச் சரிபார்த்தல்
    if (content.contains("Node(") && !content.contains("from launch_ros.actions import Node")) {
        IssueReport issue;
        issue.filePath = filePath;
        issue.lineNumber = 1;
        issue.message = "Using 'Node' without importing launch_ros.actions";
        issue.suggestedFix = "Add 'from launch_ros.actions import Node' at the top of the file.";
        issue.severity = IssueSeverity::Warning;
        issues.append(issue);
    }

    // 3. Node-ல் package அல்லது executable விடுபட்டுள்ளதா எனக் கண்டறிதல்
    QStringList lines = content.split("\n");
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        if (line.contains("Node(")) {
            if (!line.contains("package=") && !content.mid(content.indexOf(line)).left(200).contains("package=")) {
                IssueReport issue;
                issue.filePath = filePath;
                issue.lineNumber = i + 1;
                issue.message = "Node declaration might be missing 'package' or 'executable' attribute";
                issue.suggestedFix = "Ensure Node(...) specifies package='...' and executable='...'.";
                issue.severity = IssueSeverity::Warning;
                issues.append(issue);
            }
        }
    }

    return issues;
}