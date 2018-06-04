import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.2

Pane {
    id: pane
    property bool down: false
    property bool pressed: false
    property bool jhovered: false

    contentItem: Rectangle {
        id: root
        anchors.fill: parent

        // external vertical padding is 6 (to increase touch area)
        radius: 2
        color: !pane.enabled ? pane.Material.buttonDisabledColor : pane.Material.buttonColor

        Behavior on color {
            ColorAnimation {
                duration: 400
            }
        }

        // The layer is disabled when the button color is transparent so you can do
        // Material.background: "transparent" and get a proper flat button without needing
        // to set Material.elevation as well
        layer.enabled: pane.enabled && pane.Material.buttonColor.a > 0
        layer.effect: ElevationEffect {
            elevation: pane.Material.elevation
        }

        Ripple {
            clipRadius: 2
            width: parent.width
            height: parent.height
            pressed: pane.pressed
            anchor: pane
            active: pane.down || pane.visualFocus || jhovered
            color: pane.Material.rippleColor
        }
    }

    background: Rectangle { color: "transparent" }
}
