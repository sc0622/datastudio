import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'qttelnet'
    cpp.defines: [ 'QT_QTTELNET_LIB' ]

    Depends { name: 'Qt.network' }

    Properties {
        condition: qbs.targetOS.contains('windows')
        cpp.dynamicLibraries: [ 'ws2_32' ]
    }
}
