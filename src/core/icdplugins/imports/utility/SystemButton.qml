import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

ToolButton {
    id: root
    focusPolicy: Qt.NoFocus
    property string toolTip
    property alias source: image.source
    property alias sourceSize: image.sourceSize

    ToolTip.delay: 800
    ToolTip.timeout: 5000
    ToolTip.visible: toolTip.length > 0 && hovered
    ToolTip.text: toolTip

    contentItem: Image {
        id: image
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
    }
}
