import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.2
import Icd.Core 1.0

ToolButton {
    id: root
    Material.elevation: down ? 8 : 2
    font.pointSize: 10
    Layout.fillHeight: true
    readonly property variant bkColors: [ "steelblue", Qt.darker("steelblue") ]
    property JChannel rootModel: null
    property bool isRunning: false

    checked: rootModel ? rootModel.checked : false
    Material.background: (rootModel && rootModel.isOpen && isRunning)
                         ? bkColors[1] : bkColors[0]
    //text: rootModel.channel.name
    checkable: true
    implicitWidth: 60

    background: Rectangle {
        // external vertical padding is 6 (to increase touch area)
        radius: 2
        width: parent.width
        height: parent.height
        color: !root.enabled ? control.Material.buttonDisabledColor :
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
    }
}
