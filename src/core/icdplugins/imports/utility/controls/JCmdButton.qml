import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.2
import Icd.Core 1.0

RoundButton {
    id: root
    //Material.background: Qt.darker(Material.accent)
    Material.foreground: Material.accent
    property int langType: JLangModel.langType
    focusPolicy: Qt.NoFocus
    font.pointSize: getFontSize()
    font.weight: Font.Bold
    font.letterSpacing: 1

    contentItem: Label {
        text: root.text
        Layout.fillWidth: true
        Layout.fillHeight: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.RichText
        font.pointSize: root.font.pointSize
    }

    implicitHeight: 55
    //Layout.minimumHeight: 40
    leftPadding: 30
    rightPadding: 30
    //radius: 5
    /*
    background: Rectangle {
        // external vertical padding is 6 (to increase touch area)
        radius: 2
        width: parent.width
        height: parent.height
        color: !root.enabled ? root.Material.buttonDisabledColor :
                               root.highlighted ? root.Material.highlightedButtonColor
                                                : root.Material.buttonColor

        PaddedRectangle {
            y: parent.height - 4
            width: parent.width
            height: 4
            radius: 2
            topPadding: -2
            clip: true
            visible: root.checkable && (!root.highlighted || root.flat)
            color: root.checked && root.enabled ? root.Material.accentColor
                                                : root.Material.secondaryTextColor
        }

        Behavior on color {
            ColorAnimation {
                duration: 400
            }
        }

        // The layer is disabled when the button color is transparent so you can do
        // Material.background: "transparent" and get a proper flat button without needing
        // to set Material.elevation as well
        layer.enabled: root.enabled && root.Material.buttonColor.a > 0
        layer.effect: ElevationEffect {
            elevation: root.Material.elevation
        }

        Ripple {
            clipRadius: 2
            width: parent.width
            height: parent.height
            pressed: root.pressed
            anchor: root
            active: root.down || root.visualFocus || root.hovered
            color: root.Material.rippleColor
        }
    }*/

    function getFontSize() {
        var size = 0
        switch (langType) {
        case JLangModel.LangChineseSimplify: size = window.font.pointSize + 2; break
        default: size = 11; break
        }
        return size
    }
}
