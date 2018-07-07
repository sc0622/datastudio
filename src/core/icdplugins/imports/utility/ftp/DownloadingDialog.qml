import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.3
import "../../utility"
import "../../utility/controls"

Dialog {
    id: root
    focus: true
    modal: false
    //parent: ApplicationWindow.overlay
    closePolicy: Popup.NoAutoClose
    property alias progressMsg: labelProgress.text
    property string progressPath: ""
    property alias progressFrom: progress.from
    property alias progressTo: progress.to
    property alias progressValue: progress.value
    property int downloadingCount: 0
    property int downloadingIndex: -1
    Material.background: "#d0002b36"

    header: Pane {
        padding: 12
        contentItem: RowLayout {
            Image {
                id: titleIcon
                source: "../../image/ftp/wait.png"
                sourceSize: Qt.size(35, 35)
            }
            Label {
                textFormat: Text.RichText
                text: {
                    var percent = 0, totalSize = progressTo - progressFrom;
                    if (totalSize == 0) {
                        percent = 0;
                    } else {
                        percent = progressValue * 100 / totalSize
                    }
                    return "<font size=5 color=" + Material.accent + ">" + qsTr("下载文件")
                            + "</font> (<font color=yellow>" + percent.toFixed(2) + "%"
                            + generateSizeProgress(totalSize) + "</font>)"
                }
            }
            Item { Layout.fillWidth: true }
        }
    }

    FontMetrics {
        id: fontMetrics
        font.family: labelMessage.font.family
        font.pointSize: labelMessage.font.pointSize
    }

    function generateSizeProgress(totalSize) {
        if (totalSize === 0) {
            return "";
        } else {
            var calcFunc = function(size) {
                if (size < 1024) {
                    return size.toFixed(2) + "B";
                } else if (size < 1024 * 1024) {
                    return (size / 1024.0).toFixed(2) + "KB";
                } else if (size < 1024 * 1024 * 1024) {
                    return (size / (1024.0 * 1024.0)).toFixed(2) + "MB";
                } else {
                    return (size / (1024.0 * 1024.0 * 1024.)).toFixed(2) + "GB";
                }
            };
            return ",  %1 / %2".arg(calcFunc(progressValue)).arg(calcFunc(totalSize));
        }
    }

    contentItem: ColumnLayout {
        RowLayout {
            spacing: 0
            Label {
                id: labelProgress
                textFormat: Text.RichText
                visible: text.length > 0
            }
            Label {
                id: labelMessage
                Layout.fillWidth: true
                text: fontMetrics.elidedText(progressPath, Text.ElideMiddle, width, Text.Wrap)
            }
        }

        CircualProgress {
            id: progress
            clip: true
            property real from: 0
            property real to: 100
            property real value: 0
            implicitWidth: 100
            //anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignHCenter
            minimumValueAngle: -180
            maximumValueAngle: 180
            progress: {
                var length = to - from;
                if (length == 0) {
                    return 0;
                } else {
                    return value / length;
                }
            }

            ToolTip.delay: 300
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: progressPath

            background: Rectangle {
                anchors.fill: parent
                anchors.margins: 1.5
                color: "transparent"
                radius: width / 2
                border.width: 1
                border.color: parent.color
                opacity: 0.3
            }
            Image {
                anchors.centerIn: parent
                source: "../../image/ftp/downloading.png"
                sourceSize: Qt.size(80, 80)
            }
        }
        Button {
            id: buttonCancel
            text: qsTr("取消")
            focusPolicy: Qt.NoFocus
            Material.elevation: 0
            Layout.preferredWidth: 0
            Layout.fillWidth: true
            onClicked: root.reject()
            Keys.onReturnPressed: root.reject()
        }
    }

    onRejected: {
        //
    }

    Timer {
        id: timer
        repeat: true
        running: true
        interval: 100
        onTriggered: {
            titleIcon.rotation = (titleIcon.rotation + 45) % 360
        }
    }
}
