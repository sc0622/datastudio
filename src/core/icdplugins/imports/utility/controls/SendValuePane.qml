import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Icd.Core 1.0

Page {
    id: root
    property alias from: spinBox.from
    property alias to: spinBox.to
    property alias decimals: spinBox.decimals
    property alias value: spinBox.value
    property alias stepSize: spinBox.stepSize
    property string prefix: ""
    property string suffix: ""
    property alias name: buttonCmd.text
    property string command: ""

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
        color: Material.listHighlightColor
        radius: 3
    }

    contentItem: ColumnLayout {
        spacing: 0
        Label {
            id: labelTitle
            Layout.fillWidth: true
            font.weight: Font.Light
            renderType: Text.QtRendering
            opacity: 0.8
            horizontalAlignment: Text.AlignLeft
            textFormat: Text.RichText
            text: root.title
        }
        Item { Layout.fillHeight: true }
        ColumnLayout {
            JSpinBox {
                id: spinBox
                Layout.fillWidth: true
                Layout.minimumWidth: 80
                Layout.alignment: Qt.AlignHCenter
                from: 0
                to: 3
                decimals: 2
                value: 0
            }
            JCmdButton {
                id: buttonCmd
                //Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.minimumWidth: 150
                text: qsTr("--")
                onClicked: {
                    enabled = false
                    var args = prefix + (prefix.length > 0 ? " " : "")
                            + spinBox.value.toFixed(spinBox.decimals)
                    if (!JCmdModel.send(command, args, function(success, count){
                        enabled = true
                        if (success) {
                            //
                        }
                    })) {
                        enabled = true
                    }
                }
            }
        }
        Item { Layout.fillHeight: true }
    }
}
