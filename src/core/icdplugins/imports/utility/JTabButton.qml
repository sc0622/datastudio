import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Icd.Core 1.0

TabButton {
    id: root
    property int langType: JLangModel.LangChineseSimplify
    property bool selected: false
    topPadding: 0
    bottomPadding: 0
    font.bold: true
    font.pointSize: {
        var fontSize = ApplicationWindow.window
                ? ApplicationWindow.window.font.pointSize : 20
        switch (langType) {
        case JLangModel.LangChineseSimplify:
            return selected ? fontSize + 3 : fontSize + 2
        default:
            return fontSize - 2;
        }
    }

    Behavior on font.pointSize { NumberAnimation { duration: 200 } }

    contentItem: Label {
        text: parent.text
        font: parent.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: !root.enabled ? root.Material.hintTextColor
                             : root.down || root.checked
                               ? root.Material.accentColor
                               : root.Material.foreground
    }
}
