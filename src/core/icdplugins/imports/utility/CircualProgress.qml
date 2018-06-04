import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4
import QtQuick.Extras.Private.CppUtils 1.1

Button {
    id: root
    property alias progress: progressBar.progress
    property alias barWidth: progressBar.barWidth
    property alias minimumValueAngle: progressBar.minimumValueAngle
    property alias maximumValueAngle: progressBar.maximumValueAngle
    property bool __checked: false
    background: Rectangle { color: "transparent" }
    padding: 0

    signal activated

    focusPolicy: Qt.NoFocus
    implicitWidth: 60
    implicitHeight: implicitWidth

    property variant color: root.hovered
                            ? Qt.lighter(Material.accent) : Material.accent

    onColorChanged: progressBar.updateGradient()

    CircularProgressBar {
        id: progressBar
        width: Math.min(parent.width, parent.height)
        height: width
        antialiasing: true
        barWidth: 4
        minimumValueAngle: 0
        maximumValueAngle: 360

        function updateGradient() {
            clearStops()
            addStop(0, root.color)
            update()
        }

        Component.onCompleted: updateGradient()

        state: "normal"
        states: [
            State {
                name: "normal"
                PropertyChanges {
                    target: progressBar
                    opacity: 1
                }
            },
            State {
                name: "activated"
                PropertyChanges {
                    target: root
                    color: "red"
                }
            }
        ]

        transitions: [
            Transition {
                from: "normal"
                to: "activated"
                SequentialAnimation {
                    loops: Animation.Infinite
                    NumberAnimation {
                        target: progressBar
                        property: "opacity"
                        from: 0.8
                        to: 0
                        duration: 500
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        target: progressBar
                        property: "opacity"
                        from: 0
                        to: 0.8
                        duration: 500
                        easing.type: Easing.InOutSine
                    }
                }
            }
        ]
    }

    onActivated: {
        progressBar.state = "activated"
    }

    on__CheckedChanged: {
        if (!__checked) {
            progressBar.state = "normal"
        }
    }
}
