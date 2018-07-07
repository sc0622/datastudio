import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.3
import Icd.Core 1.0
import "../"

Page {
    id: root
    property JFtpModel rootModel: null
    property Window mainWindow: ApplicationWindow.window

    signal currentRowChanged(int row)

    contentItem: TableView {
        id: tableView
        frameVisible: false
        backgroundVisible: false
        alternatingRowColors: true
        sortIndicatorVisible: true
        model: rootModel ? rootModel.ftpTableModel : null
        headerDelegate: ToolBar {
            id: tableHeader
            focusPolicy: Qt.NoFocus
            width: parent.width
            height: 35
            Material.background: Qt.darker(Material.accent)
            Material.elevation: 1
            Layout.preferredWidth: 1
            contentItem: Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: styleData.value
            }
        }
        itemDelegate: RowLayout {
            id: rowLayout
            clip: true
            Item { width: 5 }
            Image {
                id: image
                visible: styleData.column === 0
                width: visible ? sourceSize.width : 0
                sourceSize: Qt.size(parent.height - 15, parent.height - 15)
                source: {
                    var source = "../../image/ftp/"
                    if (styleData.row === 0 && !rootModel.rootDir) {
                        source += "parentdir.png"
                    } else if (rootModel) {
                        if (rootModel.ftpTableModel.data(styleData.row,
                                                         JFtpTableModel.IsDirRole)) {
                            source += "folder.png"
                        } else if (rootModel.ftpTableModel.data(styleData.row,
                                                                JFtpTableModel.IsFileRole)) {
                            source += "file.png"
                        } else {
                            source += "file.png"
                        }
                    }
                    return source
                }
            }
            ProgressBar {
                id: progressBar
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true
                height: 20
                visible: {
                    if (!image.visible) {
                        return false
                    }
                    if (styleData.row === 0) {
                        return false
                    }
                    return (rootModel.downloadingRow === styleData.row)
                }
                anchors {
                    left: image.left
                    leftMargin: 10
                    right: parent.right
                }
            }
            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: styleData.column === 1
                                     ? Text.AlignRight : Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                Material.foreground: Material.accent
                text: styleData.value
                anchors {
                    left: image.right
                    leftMargin: 10
                    right: parent.right
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                visible: {
                    if (styleData.row === 0 && !rootModel.rootDir) {
                        return false;
                    } else if (rootModel) {
                        //
                    }
                    return true;
                }
                onClicked: {
                    tableView.currentRow = styleData.row
                    //menuRight.open()
                    menuRight.popup(mouseX, mouseY)
                }
            }

            Menu {
                id: menuRight
                MenuItem {
                    property bool isChecked: false
                    text: isChecked ? qsTr("取消下载") : qsTr("下载")
                    onTriggered: {
                        menuRight.close();
                        if (isChecked) {
                            isChecked = false;
                            rootModel.cancelDownload();
                        } else {
                            isChecked = true;
                            downloadRow(styleData.row, null, function(success) {
                                isChecked = false;
                            });
                        }
                    }
                }
                MenuItem {
                    property bool isChecked: false
                    text: qsTr("另存为")
                    onTriggered: {
                        menuRight.close();
                        var dir = rootModel.getSaveDir();
                        if (dir.length === 0) {
                            return;
                        }
                        visible = false;
                        downloadRow(styleData.row, dir, function(success) {
                            visible = true;
                        });
                    }
                }
            }

            Connections {
                target: rootModel
                enabled: rootModel.downloadingRow === styleData.row
                onDataTransferProgress: {
                    if (rootModel.downloadingRow !== styleData.row) {
                        return
                    }
                    progressBar.to = totalBytes
                    progressBar.value = readBytes
                }
            }
        }

        Component.onCompleted: {
            //positionViewAtRow(rowCount -1, ListView.Contain)
        }

        onClicked: {
            rootModel.processRowClicked(row)
        }

        onDoubleClicked: {
            rootModel.processRowDoubleClicked(row)
            currentRow = -1
        }

        onRowCountChanged: {
            if (rowCount == 0) {
                selection.clear();
            }
        }

        onCurrentRowChanged: {
            selection.clear();
            if (rowCount > 0 && currentRow >= 0) {
                selection.select(currentRow);
            }
            root.currentRowChanged(currentRow);
        }

        //
        focus: true
        Keys.onPressed: {
            if (!rootModel.connected) {
                connectStateNotify();
                return;
            }

            if (rootModel.downloading) {
                return;
            }
            switch (event.key) {
            case Qt.Key_Backspace:
                rootModel.cdToParent();
                currentRow = -1;
                event.accepted = true;
                break;
            case Qt.Key_Return:
            case Qt.Key_Enter:
                enterFolder();
                event.accepted = true;
                break;
            }
        }

        function enterFolder() {
            if (currentRow < 0) {
                return;
            }
            rootModel.processRowDoubleClicked(currentRow);
            currentRow = -1;
        }
    }

    BusyIndicator {
        running: rootModel ? rootModel.wait : false
        anchors.centerIn: parent
    }

    Connections {
        target: rootModel
        onConnectedChanged: {
            if (!rootModel.connected) {
                //TODO
            }
        }
    }

    function connectStateNotify() {
        notifyDialog.show(qsTr("FTP链接已断开!"));
    }

    function downloadRow(row, dir, callback) {
        var finalFunc = function(success) {
            downloadingDlg.close();
            root.enabled = true;
            if (callback) {
                callback(success);
            }
        };
        if (!rootModel.connected) {
            finalFunc(false);
            connectStateNotify();
            return;
        }
        root.enabled = false;
        downloadingDlg.start();
        if (dir === null) {
            dir = rootModel.saveAsDir;
        }
        if (!rootModel.downloadRow(row, dir, function(success){
            finalFunc(success);
        })) {
            finalFunc(false);
        }
    }

    function addColumn(column) {
        var newObject = Qt.createComponent("../JTableViewColumn.qml")
        if (newObject.status === Component.Error) {
            console.error(newObject.errorString())
            return
        }

        if (newObject.status === Component.Ready) {
            tableView.addColumn(newObject.createObject(
                                    tableView, {
                                        "title": column.title,
                                        "role": column.role,
                                        "width": column.width
                                    }))
        }
    }

    Component.onCompleted: {
        var columns = [
                    { title: qsTr("名称"), role: "name", width: 400 },
                    { title: qsTr("大小"), role: "size", width: 150 },
                    { title: qsTr("修改日期"), role: "lastModified", width: 200 },
                    //{ title: qsTr("访问日期"), role: "lastRead" },
                    { title: qsTr("拥有者"), role: "owner", width: 150 },
                    { title: qsTr("所属组"), role: "group", width: 150 }
                ]
        columns.forEach(function(column){
            addColumn(column)
        })
        //TEST
        //downloadingDlg.open()
    }

    DownloadingDialog {
        id: downloadingDlg
        parent: ApplicationWindow.overlay
        x: mainWindow ? (mainWindow.width - width) / 2 : 0
        y: mainWindow ? mainWindow.height / 5 : 0
        width: mainWindow ? Math.min(700, mainWindow.width * 0.5) : 100
        progressMsg: {
            if (downloadingCount == 0) {
                return qsTr("=> 计算下载文件夹大小...");
            } else {
                if (downloadingCount > 1) {
                    return qsTr("=> " + "<font color=" + Material.accent + ">(%1 of %2)</font>" + ": ")
                    .arg(downloadingIndex + 1).arg(downloadingCount);
                } else {
                    return "";
                }
            }
        }
        progressPath: {
            if (downloadingCount > 0) {
                return rootModel.downloadingFile;
            } else {
                return ""
            }
        }

        onRejected: {
            rootModel.cancelDownload();
        }

        function start() {
            progressFrom = 0;
            progressTo = 0;
            progressValue = 0;
            open();
        }

        onClosed: {
            downloadingCount = 0;
            downloadingIndex = 0;
        }

        Connections {
            target: rootModel
            enabled: downloadingDlg.visible
            onDataTransferProgress: {
                downloadingDlg.progressTo = totalBytes
                downloadingDlg.progressValue = readBytes
            }
            onDownloadingCountChanged: {
                downloadingDlg.downloadingCount = count
            }
            onDownloadingIndexChanged: {
                downloadingDlg.downloadingIndex = index
            }
        }
    }
}
