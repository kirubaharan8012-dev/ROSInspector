import QtQuick
import QtQuick.Controls
import ROSInspector.Renderers 1.0

Item {
    id: root
    signal closeClicked()
    property var graphData

    Native3DGraphItem {
        id: native3D
        anchors.fill: parent
        graphData: root.graphData

        MouseArea {
            anchors.fill: parent
            property point lastPos

            onPressed: function(mouse) {
                lastPos = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: function(mouse) {
                if (mouse.buttons & Qt.LeftButton) {
                    var dx = mouse.x - lastPos.x
                    var dy = mouse.y - lastPos.y
                    native3D.rotateCamera(dx, dy)
                    lastPos = Qt.point(mouse.x, mouse.y)
                }
            }

            onWheel: function(wheel) {
                native3D.zoomCamera(wheel.angleDelta.y)
            }
        }
    }

    Button {
        text: "✕ Close 3D View"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
        z: 10
        onClicked: root.closeClicked()
    }
}