import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    property int overallScore: 100
    property alias score: root.overallScore // 🟢 score மற்றும் overallScore இரண்டையும் ஏற்கும்!

    Layout.fillWidth: true
    height: 90
    color: "#1e1e2e"
    radius: 12
    border.color: "#313244"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 20

        Rectangle {
            width: 60
            height: 60
            radius: 30
            color: "transparent"
            border.color: root.overallScore > 80 ? "#a6e3a1" : (root.overallScore > 50 ? "#f9e2af" : "#f38ba8")
            border.width: 5

            Text {
                anchors.centerIn: parent
                text: root.overallScore + "%"
                font.bold: true
                font.pixelSize: 16
                color: "#cdd6f4"
            }
        }

        ColumnLayout {
            Text {
                text: "Overall ROS 2 Compatibility Score"
                font.bold: true
                font.pixelSize: 16
                color: "#cdd6f4"
            }
            Text {
                text: "Drag and drop configuration files into the dropzone below."
                font.pixelSize: 12
                color: "#a6adc8"
            }
        }
    }
}