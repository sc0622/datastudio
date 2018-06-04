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

        Pane {
            id: pane
            leftPadding: 0
            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
            width: parent.width
        }

        ScrollIndicator.horizontal: ScrollIndicator {
            id: scrollIndicatorHori
            visible: orientation & Qt.Horizontal
            Component.onCompleted: {
                if (root.alwaysVisible) flickable.binding(scrollIndicatorHori)
            }
        }

        ScrollIndicator.vertical: ScrollIndicator {
            id: scrollIndicatorVert
            visible: orientation & Qt.Vertical
            Component.onCompleted: {
                if (root.alwaysVisible) flickable.binding(scrollIndicatorVert)
            }
        }

        Connections {
            target: scrollIndicatorHori
            enabled: root.alwaysVisible
            onActiveChanged: flickable.binding(target)
        }

        Connections {
            target: scrollIndicatorVert
            enabled: root.alwaysVisible
            onActiveChanged: flickable.binding(target)
        }

        function binding(scrollIndicator) {
            scrollIndicator.active = Qt.binding(function() {
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
            }, State {
                name: "v"
                when: orientation & Qt.Vertical
                PropertyChanges {
                    target: flickable
                    contentHeight: pane.implicitHeight

                }
            }
        ]
    }
}
