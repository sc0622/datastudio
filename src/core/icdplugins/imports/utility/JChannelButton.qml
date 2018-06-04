import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.2
import Icd.Core 1.0

ToolButton {
    id: root
    //property alias channelName: channelName.text
    focusPolicy: Qt.NoFocus
    Material.elevation: down ? 8 : 2
    font.pointSize: 10
    Layout.fillHeight: true
    readonly property variant bkColors: [ "steelblue", Qt.darker("steelblue") ]
    property alias progressBar: __progressBar
    //property bool active: false
    property alias delay: __progressBar.delay
    property alias progress: __progressBar.progress
    property JCmdChannel channel: null

    checked: channel ? channel.channel.checked : false
    Material.background: (channel && channel.channel.isOpen) ? bkColors[1] : bkColors[0]
    text: channel ? channel.nativeChannel.mark : ""
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

    contentItem: RowLayout {
        Item { Layout.fillWidth: true }
        JDelayProgress {
            id: __progressBar
            implicitWidth: 35
            clickable: false
            Label {
                id: channelName
                anchors.centerIn: parent
                text: root.text
                Material.foreground: Material.accent
            }
            onClicked: {
                root.checked = !root.checked
            }
        }
        Item { Layout.fillWidth: true }
    }
/*
    Rectangle {
        id: statusBar
        color: Material.accent
        opacity: (channel && channel.channel.isOpen ? 1 : 0
        Behavior on opacity { NumberAnimation { } }
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - 2
        height: 2
    }
*//*
    //TEST
    Timer {
        id: timer
        interval: __progressBar.delay
        running: false
        triggeredOnStart: true
        onTriggered: {
            __progressBar.start()
        }
    }
    Connections {
        target: __progressBar
        onCheckedChanged: timer.restart()
    }
    Binding {
        target: __progressBar
        property: "delay"
        value: 2000 + Math.random() * 2000
    }
    Component.onCompleted: __progressBar.start()
*/
    Connections {
        target: channel
        //enabled: channel !== null
        onTimeoutStarted: {
            __progressBar.delay = msecs
            __progressBar.start()
        }
        onTimeoutStopped: {
            __progressBar.cancel()
        }
    }

    Connections {
        target: __progressBar
        onActivated: {
            if (channel) {
                channel.timeout()
            }
        }
    }
}
