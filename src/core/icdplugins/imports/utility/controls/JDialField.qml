import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Pane {
    id: root
    property alias dial: dial
    property alias title: labelTitle.text
    property alias value: dial.value
    property alias field: fieldValue
    property alias from: dial.from
    property alias to: dial.to
    property alias stepSize: dial.stepSize
    property int decimals: 0
    property string suffix: ""

    topPadding: 2
    bottomPadding: 2
    clip: true
    Layout.fillWidth: true
    Layout.minimumWidth: 60
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
            text: qsTr("--")
        }
        Dial {
            id: dial
            Layout.fillWidth: true
            onPositionChanged: {
                value = (to - from) * position
            }
            onValueChanged: {
                String.prototype.times = function(n) {
                    return Array.prototype.join.call({length: n + 1}, this)
                }
                var text = ("0".times(4) + value.toFixed(decimals).toString()).substr(0, 4) + suffix
                if (fieldValue.text != text) {
                    fieldValue.text = text
                }
            }
        }
        TextField {
            id: fieldValue
            Layout.fillWidth: true
            font.pointSize: 20
            font.weight: Font.Light
            //font.family: Flat.FlatStyle.fontFamily
            renderType: Text.QtRendering
            Material.foreground: Material.accent
            horizontalAlignment: Text.AlignHCenter
            validator: DoubleValidator {
                bottom: Math.min(dial.from, dial.to)
                top: Math.max(dial.from, dial.to)
                decimals: root.decimals
            }
            inputMask: "000.0" + suffix + ";0"
            onTextChanged: {
                var value = parseFloat(text)
                if (dial.value !== value) {
                    dial.value = value
                }
            }
        }
    }
}
