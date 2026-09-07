#include "XmlParser.h"
#include <tinyxml2.h>
#include <QFileInfo>

using namespace tinyxml2;

QVector<IssueReport> XmlParser::parsePackageXml(const QString &filePath) {
    QVector<IssueReport> issues;
    XMLDocument doc;

    XMLError error = doc.LoadFile(filePath.toUtf8().constData());
    if (error != XML_SUCCESS) {
        issues.append({
            filePath,
            doc.ErrorLineNum(),
            QString("XML Syntax Error: %1").arg(doc.ErrorStr()),
            "Fix the XML tags structure.",
            IssueSeverity::Critical
        });
        return issues;
    }

    XMLElement *root = doc.FirstChildElement("package");
    if (!root) {
        issues.append({
            filePath,
            1,
            "Missing root <package> tag in package.xml",
            "Wrap the content inside <package>...</package>.",
            IssueSeverity::Critical
        });
        return issues;
    }

    // Check format attribute
    const char *formatAttr = root->Attribute("format");
    if (!formatAttr || QString(formatAttr) != "3") {
        issues.append({
            filePath,
            1,
            "ROS 2 package.xml should use format=\"3\"",
            "Change <package> to <package format=\"3\">.",
            IssueSeverity::Warning
        });
    }

    // Check essential tags
    const char* requiredTags[] = {"name", "version", "description", "maintainer", "license"};
    for (const char* tag : requiredTags) {
        if (!root->FirstChildElement(tag)) {
            issues.append({
                filePath,
                1,
                QString("Missing required tag <%1>").arg(tag),
                QString("Add <%1>tag_content</%1>.").arg(tag),
                IssueSeverity::Critical
            });
        }
    }

    // Check buildtool_depend for ament_cmake
    XMLElement *buildtool = root->FirstChildElement("buildtool_depend");
    if (!buildtool) {
        issues.append({
            filePath,
            1,
            "Missing <buildtool_depend>ament_cmake</buildtool_depend>",
            "Add <buildtool_depend>ament_cmake</buildtool_depend> inside package.xml.",
            IssueSeverity::Warning
        });
    }

    return issues;
}