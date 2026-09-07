import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal filesDropped(var urls)
    signal maximizeClicked() // 🟢 சேர்க்கப்பட்ட சிக்னல்

    property bool isFullscreen: false

    implicitWidth: 400
    implicitHeight: 250

    Rectangle {
        id: container
        parent: root.isFullscreen ? overlayLayer : root
        anchors.fill: parent

        color: "#11111b"
        radius: root.isFullscreen ? 0 : 12
        border.color: dropArea.containsDrag ? "#89b4fa" : (root.isFullscreen ? "#a6e3a1" : "#313244")
        border.width: root.isFullscreen ? 2 : 1.5

        DropArea {
            id: dropArea
            anchors.fill: parent
            onDropped: (drop) => {
                if (drop.hasUrls) {
                    root.filesDropped(drop.urls)
                }
            }
        }

        // Native Graph Canvas
        Canvas {
            id: nativeGraphCanvas
            anchors.fill: parent
            antialiasing: true

            property var nodes: inspectorFacade ? (inspectorFacade.graphData.nodes || []) : []
            property var edges: inspectorFacade ? (inspectorFacade.graphData.edges || []) : []

            onNodesChanged: requestPaint()
            onEdgesChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);

                if (!nodes || nodes.length === 0) return;

                var centerX = width / 2;
                var centerY = height / 2;
                var radius = Math.min(width, height) * 0.35;
                var positions = {};

                for (var i = 0; i < nodes.length; i++) {
                    var angle = (i / nodes.length) * 2 * Math.PI;
                    positions[nodes[i].id] = {
                        x: centerX + radius * Math.cos(angle),
                        y: centerY + radius * Math.sin(angle),
                        score: nodes[i].score || 0,
                        label: nodes[i].label || "",
                        isForeign: nodes[i].isForeign || false
                    };
                }

                ctx.lineWidth = 1.5;
                for (var j = 0; j < edges.length; j++) {
                    var edge = edges[j];
                    var p1 = positions[edge.from];
                    var p2 = positions[edge.to];
                    if (p1 && p2) {
                        ctx.beginPath();
                        ctx.strokeStyle = edge.confidence === 100 ? "#a6e3a1" : "#f9e2af";
                        ctx.moveTo(p1.x, p1.y);
                        ctx.lineTo(p2.x, p2.y);
                        ctx.stroke();
                    }
                }

                for (var id in positions) {
                    var node = positions[id];
                    ctx.beginPath();
                    var nodeRadius = node.isForeign ? 12 : 8;
                    ctx.arc(node.x, node.y, nodeRadius, 0, 2 * Math.PI);

                    if (node.isForeign) {
                        ctx.fillStyle = "#f38ba8";
                    } else if (node.score === 100) {
                        ctx.fillStyle = "#a6e3a1";
                    } else {
                        ctx.fillStyle = "#f9e2af";
                    }
                    ctx.fill();

                    ctx.lineWidth = 2;
                    ctx.strokeStyle = "#1e1e2e";
                    ctx.stroke();

                    ctx.fillStyle = "#cdd6f4";
                    ctx.font = "11px sans-serif";
                    ctx.fillText(node.label, node.x + 12, node.y + 4);
                }
            }

            Connections {
                target: inspectorFacade
                function onGraphDataChanged() {
                    nativeGraphCanvas.requestPaint();
                }
            }
        }

        // Fullscreen Toggle Button
        Rectangle {
            id: toggleBtn
            width: root.isFullscreen ? 44 : 36
            height: root.isFullscreen ? 44 : 36
            radius: width / 2
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: root.isFullscreen ? 20 : 12
            z: 99

            color: btnMouse.containsMouse ? "#89b4fa" : "#1e1e2e"
            border.color: root.isFullscreen ? "#a6e3a1" : "#89b4fa"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: root.isFullscreen ? "↙" : "🌐"
                font.pixelSize: root.isFullscreen ? 18 : 16
                color: btnMouse.containsMouse ? "#11111b" : "#cdd6f4"
            }

            MouseArea {
                id: btnMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    root.maximizeClicked() // 🟢 3D View சிக்னல் அனுப்புகிறது
                }
            }
        }
    }

    Item {
        id: overlayLayer
        parent: Overlay.overlay
        anchors.fill: parent
    }
}