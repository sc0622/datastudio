import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.2

ItemDelegate {
    id: root
    //Material.foreground: Material.accent
    background: Rectangle {
        color: root.highlighted
               ? "steelblue" : root.Material.listHighlightColor
        Ripple {
            width: parent.width
            height: parent.height
            clip: visible
            pressed: root.pressed
            anchor: root
            active: root.down || root.visualFocus || root.hovered
            color: root.Material.rippleColor
        }
    }
}
