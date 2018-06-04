import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

GroupBox {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    clip: true
    padding: 6

    label: Label {
        text: title
        leftPadding: 10
        topPadding: -3
        bottomPadding: -5
        textFormat: Text.RichText
    }
}
