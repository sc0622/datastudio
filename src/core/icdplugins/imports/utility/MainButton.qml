import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import Icd.Core 1.0

Button {
    id: root
    focusPolicy: Qt.NoFocus
    Material.background: "#648b42"
    implicitWidth: 220
    implicitHeight: 70
    property int langType: JLangModel.langType
    font.pointSize: {
        switch (langType) {
        case JLangModel.LangChineseSimplify: return window.font.pointSize * 1.05
        default: return 11
        }
    }
    contentItem: Label {
        text: parent.text
        font: parent.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }
}
