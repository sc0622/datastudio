import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Icd.Core 1.0
import ".."

Page {
    id: root
    property JFtpModel rootModel: null

    contentItem: SplitView {
        id: splitView
        FtpMainView {
            id: mainView
            Layout.fillWidth: true
            rootModel: root.rootModel
            onCurrentRowChanged: {
                detailView.row = row
            }
        }
        FtpDetailView {
            id: detailView
            Layout.fillWidth: true
            width: parent.width / 4
            rootModel: root.rootModel ? root.rootModel.ftpTableModel : null
            visible: row >= 0
        }
    }

    Component.onDestruction: {
        if (rootModel) {
            rootModel.disconnectFromHost()
        }
    }
}
