import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

Page {
    id: root
    default property alias content: pane.contentItem
    property alias flickable: flickable
    property color paneBackgroundColor: Material.background
    implicitWidth: flickable ? flickable.contentWidth : 0
    padding: 6
    clip: true

    contentItem: Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentWidth: pane.contentWidth + pane.leftPadding + pane.rightPadding
        contentHeight: pane.contentHeight + pane.topPadding + pane.bottomPadding
        Pane {
            id: pane
            implicitWidth: flickable.width
            padding: root.padding
            background: Rectangle {
                color: paneBackgroundColor
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
        }
    }
}
