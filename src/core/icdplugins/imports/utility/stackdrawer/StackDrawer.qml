import QtQuick 2.6
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.1
import ".."

Page {
    id: root
    property int deep: 0

    contentItem: ListView {
        id: mainView
        clip: true
        Layout.fillWidth: true
        Layout.fillHeight: true
        orientation: Qt.Horizontal
        delegate: ListView {
            id: subView
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: [
                "item 1",
                "item 2"
            ]
            delegate: ItemDelegate {
                height: parent.height
                text: modelData
            }

            ScrollBar.vertical: ScrollBar { }
        }

        ScrollBar.horizontal: ScrollBar {
            id: scrollBarHori
        }
    }
}
