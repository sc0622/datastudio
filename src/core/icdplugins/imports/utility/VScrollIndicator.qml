import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root
    default property alias content: pane.contentItem
    property alias flickable: flickable
    property alias pane: pane
    implicitWidth: flickable.contentWidth
    padding: 6
    clip: true

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentWidth: pane.contentWidth + pane.leftPadding + pane.rightPadding
        contentHeight: pane.contentHeight + pane.topPadding + pane.bottomPadding
        Pane {
            id: pane
            width: flickable.width
            padding: root.padding
        }

        ScrollIndicator.vertical: ScrollIndicator {
            id: scrollIndicator
        }
    }
}
