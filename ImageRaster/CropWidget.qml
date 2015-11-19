import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id: root
    color: "white"
    width: 1240
    height: 720

    property real zoomLevel: 1.0

    function zoomIn() {
        if (zoomLevel < 3.0)
            zoomLevel += 0.1
    }

    function zoomOut() {
        if (zoomLevel > 0.5)
            zoomLevel -= 0.1
    }

    Behavior on zoomLevel {
        SpringAnimation{ duration: 100; spring: 2; damping: 0.2 }
    }

    ScrollView {
        anchors.fill: parent
        Rectangle {
            id: placeholder
            color: "black"
            width: image.width > root.width ? image.width : root.width
            height: image.height > root.height ? image.height : root.height
            Image {
                id: image
                width: zoomLevel * sourceSize.width
                height: zoomLevel * sourceSize.height
                anchors.centerIn: parent
//                source: "/Users/Leben Asa/Pictures/final_fantasy_xiii_2_cc_bb.gif"
                source: widget.image

                function updateCropArea() {
                    widget.cropArea = zoomLevel * Qt.rect(crop.x, crop.y, crop.width, crop.height)
                }

                Item {
                    id: tl
                    x: 10
                    y: 10
                }

                Item {
                    id: br
                    x: image.width - 10
                    y: image.height - 10
                }

                Rectangle {
                    id: crop
                    color: "transparent"
                    border.width: 1
                    border.color: "red"
                    anchors {
                        top: tl.top; left: tl.left
                        bottom: br.bottom; right: br.right
                    }
                    property int _m : 10
                    MouseArea {
                        id: top
                        anchors {
                            top: crop.top; left: crop.left; right: crop.right
                            leftMargin: crop._m; rightMargin: crop._m
                        }
                        height: crop._m
                        cursorShape: Qt.SplitVCursor
                        drag.target: tl
                        drag.axis: Drag.YAxis
                        drag.minimumY: 0
                        drag.maximumY: crop.y + crop.height - 2 * crop._m
                    }
                    MouseArea {
                        id: bottom
                        anchors {
                            bottom: crop.bottom; left: crop.left; right: crop.right
                            leftMargin: crop._m; rightMargin: crop._m
                        }
                        height: crop._m
                        cursorShape: Qt.SplitVCursor
                        drag.target: br
                        drag.axis: Drag.YAxis
                        drag.minimumY: crop.y + 2 * crop._m
                        drag.maximumY: image.height
                    }
                    MouseArea {
                        id: left
                        anchors {
                            left: crop.left; top: crop.top; bottom: crop.bottom
                            topMargin: crop._m; bottomMargin: crop._m
                        }
                        width: crop._m
                        cursorShape: Qt.SplitHCursor
                        drag.target: tl
                        drag.axis: Drag.XAxis
                        drag.minimumX: 0
                        drag.maximumX: crop.x + crop.width - 2 * crop._m
                    }
                    MouseArea {
                        id: right
                        anchors {
                            right: crop.right; top: crop.top; bottom: crop.bottom
                            topMargin: crop._m; bottomMargin: crop._m
                        }
                        width: crop._m
                        cursorShape: Qt.SplitHCursor
                        drag.target: br
                        drag.axis: Drag.XAxis
                        drag.minimumX: crop.x + 2 * crop._m
                        drag.maximumX: image.width
                    }

                    MouseArea {
                        id: zoomer
                        anchors.fill: parent
                        anchors.margins: crop._m
                        acceptedButtons: Qt.LeftButton
                        preventStealing: true

                        property int _lx
                        property int _ly
                        property point _ltl
                        property point _lbr

                        onWheel: {
                            if (wheel.angleDelta.y > 0)
                                zoomIn()
                            else
                                zoomOut()
                            wheel.accepted = true
                            image.updateCropArea()
                        }

                        onPressed: {
                            _lx = mouse.x
                            _ly = mouse.y
                            _ltl = Qt.point(tl.x, tl.y)
                            _lbr = Qt.point(br.x, br.y)
                            mouse.accepted = true
                            image.updateCropArea()
                        }

                        onPositionChanged: {
                            var dx = mouse.x - _lx
                            var dy = mouse.y - _ly
                            tl.x = _ltl.x + dx
                            tl.y = _ltl.y + dy
                            br.x = _lbr.x + dx
                            br.y = _lbr.y + dy
                            image.updateCropArea()
                        }
                    }

                    onXChanged: image.updateCropArea()
                    onYChanged: image.updateCropArea()
                    onWidthChanged: image.updateCropArea()
                    onHeightChanged: image.updateCropArea()
                }

                Rectangle {
                    color: "black"
                    opacity: 0.9
                    anchors {
                        top: parent.top; bottom: parent.bottom;
                        left: parent.left; right: crop.left
                    }
                }

                Rectangle {
                    color: "black"
                    opacity: 0.9
                    anchors {
                        top: parent.top; bottom: parent.bottom;
                        left: crop.right; right: parent.right
                    }
                }

                Rectangle {
                    color: "black"
                    opacity: 0.9
                    anchors {
                        top: parent.top; bottom: crop.top
                        left: crop.left; right: crop.right
                    }
                }

                Rectangle {
                    color: "black"
                    opacity: 0.9
                    anchors {
                        top: crop.bottom; bottom: parent.bottom;
                        left: crop.left; right: crop.right
                    }
                }

                Rectangle {
                    width: crop._m * 1
                    height: crop._m * 1
                    radius: crop._m * 2
                    color: crop.border.color
                    anchors.horizontalCenter: crop.horizontalCenter
                    anchors.bottom: crop.bottom
                    anchors.bottomMargin: -0.5 * height
                }

                Rectangle {
                    width: crop._m * 1
                    height: crop._m * 1
                    radius: crop._m * 2
                    color: crop.border.color
                    anchors.horizontalCenter: crop.horizontalCenter
                    anchors.top: crop.top
                    anchors.topMargin: -0.5 * height
                }

                Rectangle {
                    width: crop._m * 1
                    height: crop._m * 1
                    radius: crop._m * 2
                    color: crop.border.color
                    anchors.verticalCenter: crop.verticalCenter
                    anchors.left: crop.left
                    anchors.leftMargin: -0.5 * width
                }

                Rectangle {
                    width: crop._m * 1
                    height: crop._m * 1
                    radius: crop._m * 2
                    color: crop.border.color
                    anchors.verticalCenter: crop.verticalCenter
                    anchors.right: crop.right
                    anchors.rightMargin: -0.5 * width
                }

            }
        }
    }

    Component.onCompleted: image.updateCropArea()
}
