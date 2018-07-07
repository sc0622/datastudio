import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Page {
    id: root
    property real value: 0
    property int decimals: 0
    property string suffix: ""
    property alias titleFontSize: labelTitle.font.pointSize
    property alias fontSize: labelValue.font.pointSize
    property alias backgroundColor: bk.color
    property alias backgroundOpacity: bk.opacity
    property bool highlighting: false
    property bool highlightingFont: true
    title: qsTr("--")

    leftPadding: 8
    rightPadding: 8
    topPadding: 2
    bottomPadding: 2
    clip: true
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumWidth: 40
    Behavior on implicitWidth { NumberAnimation { } }

    background: Rectangle {
        id: bk
        color: Material.listHighlightColor
        radius: 3
    }

    contentItem: ColumnLayout {
        spacing: 0
        Label {
            id: labelTitle
            Layout.fillWidth: true
            font.weight: Font.Normal
            renderType: Text.QtRendering
            opacity: 0.8
            horizontalAlignment: Text.AlignLeft
            text: root.title
        }
        Item { Layout.fillHeight: true }
        RowLayout {
            Item { Layout.fillWidth: true }
            Label {
                id: labelValue
                font.pointSize: 18
                font.weight: Font.Normal
                //font.family: Flat.FlatStyle.fontFamily
                renderType: Text.QtRendering
                Material.foreground: (highlightingFont && highlighting)
                                     ? "red" : Material.accent
                text: value.toFixed(decimals).toString()
                Label {
                    id: labelSuffix
                    font.pointSize: fontSize * 2 / 3
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
        Item { Layout.fillHeight: true }
    }
}
