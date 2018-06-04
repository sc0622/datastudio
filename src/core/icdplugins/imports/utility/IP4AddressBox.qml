import QtQuick 2.9
import QtQuick.Controls 1.4 as T
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Item {
    id: root
    implicitWidth: 200
    implicitHeight: 30

    property string text: "0.0.0.0"

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: 2
        anchors.rightMargin: 2
        spacing: 0
        TextField {
            id: ip1
            Material.foreground: Material.accent
            implicitWidth: (parent.width - 9) / 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            validator: IntValidator { bottom: 0; top: 255 }
            selectByMouse: true
            maximumLength: 3
            onTextChanged: {
                updateText(0, text)

                if (acceptNextFocus(text)) {
                    ip2.focus = true
                    ip2.selectAll()
                }
            }
        }
        Rectangle {
            width: 3
            height: 3
            radius: 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 8
            color: Material.foreground
        }
        TextField {
            id: ip2
            Material.foreground: Material.accent
            implicitWidth: (parent.width - 9) / 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            validator: IntValidator { bottom: 0; top: 255 }
            selectByMouse: true
            maximumLength: 3
            onTextChanged: {
                updateText(1, text)

                if (acceptNextFocus(text)) {
                    ip3.focus = true
                    ip3.selectAll()
                }
            }
        }
        Rectangle {
            width: 3
            height: 3
            radius: 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 8
            color: Material.foreground
        }
        TextField {
            id: ip3
            Material.foreground: Material.accent
            implicitWidth: (parent.width - 9) / 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            validator: IntValidator { bottom: 0; top: 255 }
            selectByMouse: true
            maximumLength: 3
            onTextChanged: {
                updateText(2, text)

                if (acceptNextFocus(text)) {
                    ip4.focus = true
                    ip4.selectAll()
                }
            }
        }
        Rectangle {
            width: 3
            height: 3
            radius: 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 8
            color: Material.foreground
        }
        TextField {
            id: ip4
            Material.foreground: Material.accent
            implicitWidth: (parent.width - 9) / 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            validator: IntValidator { bottom: 0; top: 255 }
            selectByMouse: true
            maximumLength: 3
            onTextChanged: {
                updateText(3, text)
            }
        }
    }

    function acceptNextFocus(text) {
        if (text.length === 3) {
            return true
        }

        var value = parseInt(text)
        if (value > 25 || value === 255) {
            return true
        }

        return false
    }

    function updateText(section, text) {
        var sections = root.text.split('.')
        if (sections.length < 4) {
            return false
        }

        if (section < 0 || section > 3) {
            return false
        }

        if (text.length === 0) {
            text = "0"
        }

        if (sections[section] === text) {
            return true
        }

        sections[section] = text

        root.text = sections.join('.')
    }

    onTextChanged: {
        var sections = text.split('.')
        if (sections.length != 4) {
            console.log("Invalid IP! (" + text + ")")
            return;
        }
        ip1.text = sections[0]
        ip2.text = sections[1]
        ip3.text = sections[2]
        ip4.text = sections[3]
    }
}
