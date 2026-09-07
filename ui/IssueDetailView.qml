import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: scrollView
    clip: true

    // 🟢 Property பெயர் 'issuesModel' என மாற்றப்பட்டது & Signal அறிவிக்கப்பட்டது
    property var issuesModel: []
    signal applyFixClicked(string filePath, int lineNumber, string message)

    ListView {
        id: listView
        model: scrollView.issuesModel
        spacing: 10

        delegate: Rectangle {
            width: listView.width
            height: layout.implicitHeight + 20
            color: "#252538"
            radius: 8
            border.color: modelData.severity === 2 ? "#f38ba8" : (modelData.severity === 1 ? "#f9e2af" : "#89b4fa")
            border.width: 1

            ColumnLayout {
                id: layout
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                RowLayout {
                    Layout.fillWidth: true

                    Rectangle {
                        width: 70
                        height: 22
                        radius: 4
                        color: modelData.severity === 2 ? "#f38ba8" : (modelData.severity === 1 ? "#f9e2af" : "#89b4fa")

                        Text {
                            anchors.centerIn: parent
                            text: modelData.severity === 2 ? "CRITICAL" : (modelData.severity === 1 ? "WARNING" : "INFO")
                            color: "#11111b"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }

                    Text {
                        text: modelData.filePath + " : Line " + modelData.lineNumber
                        color: "#a6adc8"
                        font.pixelSize: 12
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                    }
                }

                Text {
                    text: modelData.message
                    color: "#cdd6f4"
                    font.pixelSize: 14
                    font.bold: true
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Text {
                    text: "Suggested Fix: " + modelData.suggestedFix
                    color: "#a6e3a1"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Button {
                    text: "Apply Auto-Fix"
                    Layout.alignment: Qt.AlignRight
                    onClicked: {
                        scrollView.applyFixClicked(modelData.filePath, modelData.lineNumber, modelData.message)
                    }
                }
            }
        }
    }
}