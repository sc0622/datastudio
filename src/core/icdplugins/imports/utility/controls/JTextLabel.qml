import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Page {
    id: root
    property alias value: labelValue.text
    property alias titleFontSize: labelTitle.font.pointSize
    property alias fontSize: labelValue.font.pointSize
    property alias backgroundColor: bk.color
    property alias backgroundOpacity: bk.opacity
    property alias titleTextFormat: labelTitle.textFormat
    property alias valueTextFormat: labelValue.textFormat
    property int textFormat: labelValue.textFormat
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
        color: (!highlightingFont && highlighting)
               ? "#466284"
               : Material.listHighlightColor
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
        Label {
            id: labelValue
            Layout.fillWidth: true
            font.pointSize: 18
            font.weight: Font.Normal
            renderType: Text.QtRendering
            Material.foreground: (highlightingFont && highlighting)
                                 ? "red" : Material.accent
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("--")
        }
        Item { Layout.fillHeight: true }
    }
}
