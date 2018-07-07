import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Page {
    id: root
    property alias dial: dial
    property alias value: dial.value
    property alias from: dial.from
    property alias to: dial.to
    property alias stepSize: dial.stepSize
    property alias fontSize: labelValue.font.pointSize
    property alias flow: gridLayout.flow
    property int decimals: 0
    property string suffix: ""
    title: qsTr("--")

    leftPadding: 8
    rightPadding: 8
    topPadding: 2
    bottomPadding: 6
    clip: true
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumWidth: 40
    Behavior on implicitWidth { NumberAnimation { } }

    background: Rectangle {
        color: Material.listHighlightColor
        radius: 3
    }

    contentItem: GridLayout {
        id: gridLayout
        columns: flow == gridLayout.LeftToRight ? 2 : 1
        flow: GridLayout.TopToBottom
        Label {
            id: labelTitle
            Layout.fillWidth: true
            font.weight: Font.Normal
            renderType: Text.QtRendering
            opacity: 0.8
            horizontalAlignment: Text.AlignLeft
            text: root.title
        }
        RowLayout {
            Item { Layout.fillWidth: true }
            Dial {
                id: dial
                implicitHeight: 65
                onPositionChanged: {
                    value = (to - from) * position
                }
            }
            Item { width: 3 }
            Label {
                id: labelValue
                font.pointSize: 18
                font.weight: Font.Normal
                //font.family: Flat.FlatStyle.fontFamily
                renderType: Text.QtRendering
                Material.foreground: Material.accent
                horizontalAlignment: Text.AlignHCenter
                text: dial.value.toFixed(decimals)
                Label {
                    id: labelSuffix
                    font.pointSize: fontSize / 2
                    font.weight: Font.Normal
                    font.family: 'Noto Sans S Chinese Bold'
                    renderType: Text.QtRendering
                    Material.foreground: Material.accent
                    anchors {
                        left: parent.right
                        top: parent.top
                        leftMargin: 2
                    }
                    text: suffix
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
}
