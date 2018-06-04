import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Icd.Core 1.0
import "../"

Page {
    id: root
    clip: true
    padding: 6
    property int row: -1
    property JFtpTableModel rootModel: null

    contentItem: ColumnLayout {
        GridLayout {
            columns: 2
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr("名称：")
            }
            Label {
                id: labelName
                Layout.fillWidth: true
                Material.foreground: Material.accent
            }
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr("大小：")
            }
            Label {
                id: labelSize
                Layout.fillWidth: true
                Material.foreground: Material.accent
            }
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr("修改日期：")
            }
            Label {
                id: labelLastModified
                Layout.fillWidth: true
                Material.foreground: Material.accent
            }
        }
        Item { Layout.fillHeight: true }
    }

    onRowChanged: {
        if (row < 0) {
            labelName.text = ""
            labelSize.text = ""
            labelLastModified.text = ""
        } else {
            // name
            var name = rootModel.data(row, JFtpTableModel.NameRole)
            if (name) {
                labelName.text = name
            } else {
                labelName.text = ""
            }
            // size
            var size = rootModel.data(row, JFtpTableModel.SizeRole)
            if (size) {
                labelSize.text = size
            } else {
                labelSize.text = ""
            }
            // lastModified
            var lastModified = rootModel.data(row, JFtpTableModel.LastModifiedRole)
            if (lastModified) {
                labelLastModified.text = lastModified
            } else {
                labelLastModified.text = ""
            }
        }
    }
}
