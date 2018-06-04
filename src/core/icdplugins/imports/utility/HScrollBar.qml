import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root
    default property alias content: pane.contentItem
    property alias flickable: flickable
    property alias pane: pane
    implicitHeight: flickable.contentHeight
    padding: 6
    clip: true

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.HorizontalFlick
        contentWidth: pane.contentWidth + pane.leftPadding + pane.rightPadding
        contentHeight: pane.contentHeight + pane.topPadding + pane.bottomPadding
        Pane {
            id: pane
            height: flickable.height
            padding: root.padding
        }

        ScrollBar.horizontal: ScrollBar {
            id: scrollBar
        }
    }
}
