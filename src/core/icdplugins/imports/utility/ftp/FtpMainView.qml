import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Icd.Core 1.0
import "../"
import "../controls"
import "ftp.js" as FtpJs

Page {
    id: root
    padding: 6
    property JFtpModel rootModel: null
    Behavior on implicitWidth { NumberAnimation { } }

    signal currentRowChanged(int row)

    contentItem: ColumnLayout {
        enabled: rootModel ? !rootModel.downloading : false
        JGroupBox {
            title: qsTr("设置")
            Layout.fillHeight: false
            contentItem: ColumnLayout {
                RowLayout {
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("下载保存位置：")
                    }
                    TextField {
                        id: fieldDownloadDir
                        Layout.fillWidth: true
                        selectByMouse: true
                        Material.foreground: Material.accent
                        ToolTip.delay: 800
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: text
                        Component.onCompleted: {
                            if (rootModel) {
                                text = rootModel.saveAsDir
                            }
                        }
                        onTextChanged: {
                            if (rootModel) {
                                rootModel.saveAsDir = text
                            }
                        }
                    }
                    Button {
                        id: buttonSaveAs
                        text: qsTr("…")
                        onClicked: {
                            var dir = rootModel.getSaveDir()
                            if (dir.length === 0) {
                                return
                            }
                            fieldDownloadDir.text = dir
                        }
                    }
                }
            }
        }
        FtpListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            rootModel: root.rootModel
            onCurrentRowChanged: {
                root.currentRowChanged(row)
            }
        }
    }
}
