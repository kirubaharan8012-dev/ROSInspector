#include "UrdfParser.h"
#include <tinyxml2.h>
#include <QSet>

using namespace tinyxml2;

QVector<IssueReport> UrdfParser::parseUrdfXacro(const QString &filePath) {
    QVector<IssueReport> issues;
    XMLDocument doc;

    XMLError error = doc.LoadFile(filePath.toUtf8().constData());
    if (error != XML_SUCCESS) {
        issues.append({
            filePath,
            doc.ErrorLineNum(),
            QString("Xacro/URDF XML Error: %1").arg(doc.ErrorStr()),
            "Fix unclosed tags or syntax formatting in Xacro/URDF.",
            IssueSeverity::Critical
        });
        return issues;
    }

    XMLElement *robot = doc.FirstChildElement("robot");
    if (!robot) {
        issues.append({
            filePath,
            1,
            "Missing root <robot> tag in URDF/Xacro file",
            "Wrap model content inside <robot name=\"...\">...</robot>.",
            IssueSeverity::Critical
        });
        return issues;
    }

    QSet<QString> linkNames;
    for (XMLElement *link = robot->FirstChildElement("link"); link; link = link->NextSiblingElement("link")) {
        const char *name = link->Attribute("name");
        if (name) {
            QString nameStr(name);
            if (linkNames.contains(nameStr)) {
                issues.append({
                    filePath,
                    link->GetLineNum(),
                    QString("Duplicate link name found: '%1'").arg(nameStr),
                    "Ensure each link has a unique name identifier.",
                    IssueSeverity::Critical
                });
            } else {
                linkNames.insert(nameStr);
            }
        }

        // Inertia zero value check
        XMLElement *inertial = link->FirstChildElement("inertial");
        if (inertial) {
            XMLElement *inertia = inertial->FirstChildElement("inertia");
            if (inertia) {
                double ixx = inertia->DoubleAttribute("ixx", 0.0);
                double iyy = inertia->DoubleAttribute("iyy", 0.0);
                double izz = inertia->DoubleAttribute("izz", 0.0);
                if (ixx == 0.0 && iyy == 0.0 && izz == 0.0) {
                    issues.append({
                        filePath,
                        inertia->GetLineNum(),
                        QString("Zero inertia values detected in link '%1'").arg(name ? name : "unknown"),
                        "Provide non-zero inertia values for stable Gazebo physics simulation.",
                        IssueSeverity::Warning
                    });
                }
            }
        }
    }

    return issues;
}