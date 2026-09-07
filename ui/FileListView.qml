import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property alias model: listView.model
    property int selectedIndex: -1

    // 🟢 சேர்க்கப்பட்ட சிக்னல் (main.qml-ல் உள்ள onFileSelected இயங்க இது அவசியம்)
    signal fileSelected(string filePath)

    color: "#1e1e2e"
    radius: 12
    border.color: "#313244"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        Text {
            text: "Analyzed Files (Click to inspect)"
            font.bold: true
            font.pixelSize: 15
            color: "#cdd6f4"
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 42
                color: root.selectedIndex === index ? "#313244" : "#181825"
                radius: 8
                border.color: root.selectedIndex === index ? "#89b4fa" : "#313244"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.selectedIndex = index;
                        // 🟢 main.qml-க்கு சிக்னல் அனுப்புகிறது
                        root.fileSelected(modelData.filePath);
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: modelData.fileName
                        color: "#cdd6f4"
                        font.pixelSize: 13
                        Layout.fillWidth: true
                        elide: Text.ElideMiddle
                    }

                    Rectangle {
                        width: 45
                        height: 22
                        radius: 5
                        color: modelData.score > 80 ? "#a6e3a1" : (modelData.score > 50 ? "#f9e2af" : "#f38ba8")

                        Text {
                            anchors.centerIn: parent
                            text: modelData.score + "%"
                            font.bold: true
                            font.pixelSize: 11
                            color: "#11111b"
                        }
                    }
                }
            }
        }
    }
}