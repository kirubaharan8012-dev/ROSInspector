import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: diffPopup
    width: 700
    height: 500
    modal: true
    focus: true
    anchors.centerIn: Overlay.overlay

    property string originalText: ""
    property string fixedText: ""

    background: Rectangle {
        color: "#181825"
        border.color: "#313244"
        radius: 12
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        Text {
            text: "Review Auto-Fix Changes"
            color: "#cdd6f4"
            font.pixelSize: 16
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            // Original Code
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text { text: "Original File"; color: "#f38ba8"; font.bold: true }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    TextArea {
                        text: originalText
                        readOnly: true
                        color: "#cdd6f4"
                        background: Rectangle { color: "#1e1e2e" }
                    }
                }
            }

            // Proposed Fix Code
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text { text: "Proposed Fix"; color: "#a6e3a1"; font.bold: true }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    TextArea {
                        text: fixedText
                        readOnly: true
                        color: "#cdd6f4"
                        background: Rectangle { color: "#1e1e2e" }
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 10

            Button {
                text: "Cancel"
                onClicked: diffPopup.close()
            }

            Button {
                text: "Confirm & Save"
                highlighted: true
                onClicked: {
                    // Confirm write operation
                    diffPopup.close()
                }
            }
        }
    }
}