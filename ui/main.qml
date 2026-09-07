import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 800
    title: "ROSInspector - System Health"
    color: "#0f111a"

    property bool is3DFullscreen: false

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // 1. Header Module
        HeaderScore {
            Layout.fillWidth: true
            overallScore: inspectorFacade.overallScore
        }

        // 2. Main Content Split
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // Left Side: DropZone
            DropZone {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                
                onMaximizeClicked: root.is3DFullscreen = true
                onFilesDropped: function(urls) {
                    inspectorFacade.inspectFiles(urls)
                }
            }

            // Right Side: File List & Details Stack
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                spacing: 16

                FileListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: inspectorFacade.fileListModel
                    onFileSelected: function(path) { inspectorFacade.selectFileForInspection(path) }
                }

                IssueDetailView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    issuesModel: inspectorFacade.issues
                    onApplyFixClicked: function(path, line, msg) { inspectorFacade.applyAutoFix(path, line, msg) }
                }
            }
        }
    }

    // 3. 3D Overlay Module
    Graph3DView {
        anchors.fill: parent
        visible: root.is3DFullscreen
        graphData: inspectorFacade.graphData
        onCloseClicked: root.is3DFullscreen = false
    }
}