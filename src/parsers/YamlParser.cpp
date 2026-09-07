#include "YamlParser.h"
#include <yaml-cpp/yaml.h>
#include <QFileInfo>

QVector<IssueReport> YamlParser::parseYamlParams(const QString &filePath) {
    QVector<IssueReport> issues;

    try {
        YAML::Node config = YAML::LoadFile(filePath.toUtf8().constData());
        
        if (!config.IsMap()) {
            issues.append({
                filePath,
                1,
                "YAML root is not a valid map structure",
                "Ensure the YAML file starts with node names mapping.",
                IssueSeverity::Critical
            });
            return issues;
        }

        // ROS 2 Parameter structure check: node_name -> ros__parameters
        bool hasRosParams = false;
        for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
            YAML::Node nodeContent = it->second;

            if (nodeContent.IsMap() && nodeContent["ros__parameters"]) {
                hasRosParams = true;
                break;
            }
        }

        if (!hasRosParams) {
            issues.append({
                filePath,
                1,
                "Missing 'ros__parameters' key in ROS 2 YAML configuration",
                "Format YAML as:\nnode_name:\n  ros__parameters:\n    param_name: value",
                IssueSeverity::Warning
            });
        }

    } catch (const YAML::Exception &e) {
        issues.append({
            filePath,
            e.mark.line > 0 ? e.mark.line : 1,
            QString("YAML Syntax Error: %1").arg(e.what()),
            "Fix YAML indentation and syntax formatting.",
            IssueSeverity::Critical
        });
    }

    return issues;
}