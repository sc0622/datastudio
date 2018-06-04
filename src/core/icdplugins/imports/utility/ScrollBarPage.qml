import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root
    default property alias content: pane.contentItem
    property alias flickable: flickable
    property bool alwaysVisible: false
    property int orientation: Qt.Vertical

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.AutoFlickIfNeeded
        onContentHeightChanged: {
            scrollToBottom()
        }
        Pane {
            id: pane
            leftPadding: 0
            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
        }

        ScrollBar.horizontal: ScrollBar {
            id: scrollBarHori
            visible: orientation & Qt.Horizontal
            height: 10
            Component.onCompleted: {
                if (root.alwaysVisible) flickable.binding(scrollBarHori)
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollBarVert
            visible: orientation & Qt.Vertical
            width: 10
            Component.onCompleted: {
                if (root.alwaysVisible) flickable.binding(scrollBarVert)
            }
        }

        Connections {
            target: scrollBarHori
            enabled: root.alwaysVisible
            onActiveChanged: flickable.binding(target)
        }

        Connections {
            target: scrollBarVert
            enabled: root.alwaysVisible
            onActiveChanged: flickable.binding(target)
        }

        function binding(scrollBar) {
            scrollBar.active = Qt.binding(function() {
                return true
            })
        }

        states: [
            State {
                name: "h"
                when: orientation & Qt.Horizontal
                PropertyChanges {
                    target: flickable
                    contentWidth: pane.implicitWidth

                }
                PropertyChanges {
                    target: pane
                    height: flickable.height
                }
            }, State {
                name: "v"
                when: orientation & Qt.Vertical
                PropertyChanges {
                    target: flickable
                    contentHeight: pane.implicitHeight

                }
                PropertyChanges {
                    target: pane
                    width: flickable.width
                }
            }
        ]
    }

    function scrollToTop() {
        scrollBarVert.position = 0
    }

    function scrollToBottom() {
        scrollBarVert.position = 1
    }
}
