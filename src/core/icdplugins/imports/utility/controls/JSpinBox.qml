import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Styles 1.4 as Base
import QtQuick.Controls.Styles.Flat 1.0
import QtQuick.Controls.Private 1.0

SpinBox {
    id: root
    property alias from: root.minimumValue
    property alias to: root.maximumValue
    property bool indicatorVisible: true
    selectByMouse: true
    stepSize: 1 / Math.pow(10, decimals)
    implicitWidth: 150
    implicitHeight: 50
    font.pointSize: ApplicationWindow.window ? ApplicationWindow.window.font.pointSize + 2 : 10

    style: Base.SpinBoxStyle {
        padding { top: 10; left: 2; right: 2; bottom: 2 }
        renderType: FlatStyle.__renderType

        panel: Item {
            property int horizontalAlignment: Qt.AlignHCenter
            property int verticalAlignment: Qt.AlignVCenter

            property color foregroundColor: !control.enabled ? FlatStyle.mediumFrameColor : Material.accent
            property color selectionColor: Qt.darker(Material.accent)
            property color selectedTextColor: Material.foreground
            property var margins: QtObject {
                readonly property real top: 2 * FlatStyle.scaleFactor
                readonly property real left: decrement.width
                readonly property real right: increment.width
                readonly property real bottom: 2 * FlatStyle.scaleFactor
            }

            property rect upRect: Qt.rect(increment.x, increment.y, increment.width, increment.height)
            property rect downRect: Qt.rect(decrement.x, decrement.y, decrement.width, decrement.height)

            property font font
            font.family: FlatStyle.fontFamily

            implicitWidth: Math.round(100 * FlatStyle.scaleFactor)
            implicitHeight: Math.round(26 * FlatStyle.scaleFactor)

            Item {
                id: decrement
                visible: indicatorVisible
                clip: true
                width: visible ? Math.round(35 * FlatStyle.scaleFactor) : 0
                height: width
                anchors {
                    top: parent.top
                    topMargin: 10
                }

                RippleRectangle {
                    id: rippleDecrement
                    height: parent.height
                    width: height
                    enabled: (root.value > root.minimumValue)
                    down: styleData.downPressed
                    pressed: styleData.downPressed
                    jhovered: styleData.downHovered
                }

                Rectangle {
                    color: rippleDecrement.enabled ? Material.accent : "gray"
                    width: Math.round(15 * FlatStyle.scaleFactor)
                    height: Math.round(2 * FlatStyle.scaleFactor)
                    anchors.horizontalCenter: parent.left
                    anchors.horizontalCenterOffset: Math.round(16 * FlatStyle.scaleFactor)
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Item {
                id: increment
                clip: true
                visible: indicatorVisible
                width: visible ? Math.round(35 * FlatStyle.scaleFactor) : 0
                height: width
                anchors {
                    top: parent.top
                    topMargin: 10
                    right: parent.right
                }

                RippleRectangle {
                    id: rippleIncrement
                    height: parent.height
                    width: height
                    anchors.right: parent.right
                    enabled: (root.value < root.maximumValue)
                    down: styleData.upPressed
                    pressed: styleData.upPressed
                    jhovered: styleData.upHovered
                }

                Rectangle {
                    color: rippleIncrement.enabled ? Material.accent : "gray"
                    width: Math.round(15 * FlatStyle.scaleFactor)
                    height: Math.round(2 * FlatStyle.scaleFactor)
                    anchors.horizontalCenter: parent.right
                    anchors.horizontalCenterOffset: Math.round(-16 * FlatStyle.scaleFactor)
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    color: rippleIncrement.enabled ? Material.accent : "gray"
                    width: Math.round(2 * FlatStyle.scaleFactor)
                    height: Math.round(15 * FlatStyle.scaleFactor)
                    anchors.horizontalCenter: parent.right
                    anchors.horizontalCenterOffset: Math.round(-16 * FlatStyle.scaleFactor)
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                id: bottomLine
                anchors {
                    left: decrement.right
                    right: increment.left
                    bottom: parent.bottom
                }
                height: control.enabled && control.activeFocus ? 2 : 1
                color: !control.enabled ? "transparent" :
                                          control.activeFocus ? Material.accent : "gray"
            }
        }

        __selectionHandle: null
        __cursorHandle: null
    }
}
