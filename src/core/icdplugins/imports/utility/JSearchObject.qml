import QtQuick 2.9
import Icd.Core 1.0
import "utility.js" as UtilitiyJs

QtObject {
    id: root

    Component.onDestruction: {
        UtilitiyJs.cancelSearch(root)
    }
}
