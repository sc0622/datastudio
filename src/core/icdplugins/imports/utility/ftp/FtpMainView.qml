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
                    visible: false
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("主机（FTP）：")
                    }
                    TextField {
                        id: fieldHost
                        property string prefix: ""
                        Layout.fillWidth: true
                        selectByMouse: true
                        Material.foreground: Material.accent
                        ToolTip.delay: 800
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: text
                        Component.onCompleted: {
                            text = FtpJs.values.host
                            prefix = text
                        }
                        Component.onDestruction: {
                            FtpJs.values.host = text
                        }
                    }
                    Item { width: 30}
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("端口：")
                    }
                    JSpinBox {
                        id: spinPort
                        indicatorVisible: false
                        from: 0
                        to: 65535
                        value: 21
                        Component.onCompleted: {
                            value = FtpJs.values.port
                        }
                        Component.onDestruction: {
                            FtpJs.values.port = value
                        }
                    }
                }
                RowLayout {
                    visible: false
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("用户名：")
                    }
                    TextField {
                        id: fieldUser
                        Layout.fillWidth: true
                        selectByMouse: true
                        Component.onCompleted: {
                            text = FtpJs.values.userName
                        }
                        Component.onDestruction: {
                            FtpJs.values.userName = text
                        }
                    }
                    Item { width: 30}
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("密码：")
                    }
                    TextField {
                        id: fieldPassword
                        Layout.fillWidth: true
                        selectByMouse: true
                        echoMode: TextInput.Password
                        Component.onCompleted: {
                            text = FtpJs.values.password
                        }
                        Component.onDestruction: {
                            FtpJs.values.password = text
                        }
                    }
                    Item { width: 30 }
                    Button {
                        id: buttonLogin
                        property bool isConnected: false
                        Material.background: Qt.darker(Material.accent)
                        implicitWidth: 120
                        text: isConnected ? qsTr("断开连接") : qsTr("连接")
                        onClicked: {
                            var execCallback = function(success) {
                                if (success) {
                                    isConnected = !isConnected
                                }
                            }
                            if (isConnected) {
                                rootModel.disconnectFromHost(execCallback)
                            } else {
                                var text = fieldHost.text
                                text = text.replace(/[^:]\/+/g, function(word){
                                    if (word.length === 0) {
                                        return ""
                                    }
                                    return word.charAt(0) + "/"
                                })
                                text = text.replace(/\\/g, '/')
                                fieldHost.text = text
                                fieldHost.prefix = fieldHost.text
                                rootModel.connectToHost(fieldHost.text, spinPort.value,
                                                        fieldUser.text, fieldPassword.text,
                                                        execCallback)
                            }
                        }
                    }
                }
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

    Connections {
        target: rootModel
        onCurrentPathChanged: {
            var text = fieldHost.prefix + rootModel.currentPath
            fieldHost.text = text.replace(/[^:]\/+/g, function(word){
                if (word.length === 0) {
                    return ""
                }
                return word.charAt(0) + "/"
            })
        }
/*
        onStateChanged: {
            switch (state) {
            case FtpModel.Unconnected:
                break
            case FtpModel.HostLookup:
                break
            case FtpModel.Connecting:
                break
            case FtpModel.Connected:
                break
            case FtpModel.LoggedIn:
                break
            case FtpModel.Closing:
                break
            default:
                break
            }
            console.log(state)
        }*/
    }
}
